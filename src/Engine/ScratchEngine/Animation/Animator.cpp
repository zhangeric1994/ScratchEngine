#include "Animator.h"


ScratchEngine::AnimationBlender::AnimationBlender()
{
}

ScratchEngine::AnimationBlender::AnimationBlender(vector<XMMATRIX> first, vector<XMMATRIX> second)
{
	SetData(first, second);
}

vector<XMMATRIX> ScratchEngine::AnimationBlender::operator()(f32 blendFactor)
{
	blendFactor = __min(1, blendFactor);

	vector<XMMATRIX> output;

	for (int i = 0; i < firstTranslation.size(); ++i)
		output.push_back(XMMatrixTranspose(XMMatrixScalingFromVector(XMVectorLerp(firstScale[i], secondScale[i], blendFactor)) * XMMatrixRotationQuaternion(XMQuaternionSlerp(firstRotation[i], secondRotation[i], blendFactor)) * XMMatrixTranslationFromVector(XMVectorLerp(firstTranslation[i], secondTranslation[i], blendFactor))));

	return output;
}

__forceinline void ScratchEngine::AnimationBlender::SetData(vector<XMMATRIX> first, vector<XMMATRIX> second)
{
	assert(first.size() == second.size());

	firstTranslation.clear();
	firstRotation.clear();
	firstScale.clear();
	secondTranslation.clear();
	secondRotation.clear();
	secondScale.clear();

	for (int i = 0; i < first.size(); ++i)
	{
		XMMATRIX firstWorldMatrix = XMMatrixTranspose(first[i]);
		firstTranslation.push_back(firstWorldMatrix.r[3]);
		firstRotation.push_back(XMQuaternionRotationMatrix(firstWorldMatrix));
		firstScale.push_back(XMVectorSet(1, 1, 1, 1));

		XMMATRIX secondWorldMatrix = XMMatrixTranspose(second[i]);
		secondTranslation.push_back(secondWorldMatrix.r[3]);
		secondRotation.push_back(XMQuaternionRotationMatrix(secondWorldMatrix));
		secondScale.push_back(XMVectorSet(1, 1, 1, 1));
	}
}


ScratchEngine::Animator::Animator()
{
	timePos = 0.0f;
	skeleton = nullptr;
	LoopClips = true;
	previousAnimationIndex = null_index;
	currentAnimationIndex = null_index;
	hasSkeleton = false;

	blendFactor = 1;
	blendSpeed = 10;
}


ScratchEngine::Animator::Animator(const aiScene* scene)
{	
	timePos = 0.0f;
	LoopClips = true;

	previousAnimationIndex = null_index;
	currentAnimationIndex = null_index;

	blendFactor = 1;
	blendSpeed = 10;

	if (!scene->HasAnimations()) {
		// no animation
		skeleton = nullptr;
		currentAnimationIndex = null_index;
		hasSkeleton = false;
		return;
	}
	hasSkeleton = true;
	skeleton = CreateBoneTree(scene->mRootNode, nullptr);

	for (UINT i = 0; i < scene->mNumMeshes; i++){
		aiMesh * mesh = scene->mMeshes[i];
		for (UINT j = 0; j < mesh->mNumBones; j++) {
			aiBone * bone = mesh->mBones[j];
			std::map<std::string, Bone *>::iterator it = bonesByName.find(std::string(bone->mName.C_Str()));
			if (it == bonesByName.end()) {
				// can not find the correspond bones in structure
				continue;
			}
			bool skip = false;
			for (int i = 0; i < bones.size(); i++) {
				Bone* t = bones[i];
				if (t->name == bone->mName.C_Str()) {
					skip = true;
					break;
				}
			}
			if (skip) continue;
			Bone * found = it->second;
			found->offset = XMMatrixTranspose(ToMatrix(bone->mOffsetMatrix));
			bones.push_back(found);
			bonesToIndex[found->name] = bones.size()-1;
		}
		// TODO may come back if any bone is missing

	}

	LoadAnimations(scene);

	printf("Finished loading animations with %d bones", bones.size());
}

ScratchEngine::Animator::~Animator()
{
	delete skeleton;
}


void ScratchEngine::Animator::SetAnimationIndex(int animIndex)
{
	animIndex = __max(null_index, animIndex);

	if (animIndex >= animations.size())
		return;

	if (animIndex == null_index)
	{
		previousAnimationIndex = null_index;
		currentAnimationIndex = null_index;
	}
	else if (animIndex != currentAnimationIndex)
	{
		previousAnimationIndex = currentAnimationIndex;
		currentAnimationIndex = animIndex;

		if (blendFactor < 1)
		{
			blender.SetData(blender(blendFactor), animations[currentAnimationIndex]->GetTransforms(1 / blendSpeed));
			blendFactor = 0;
		}
		else if (previousAnimationIndex != null_index)
		{
			blender.SetData(animations[previousAnimationIndex]->GetTransforms(timePos), animations[currentAnimationIndex]->GetTransforms(1));
			blendFactor = 0;
		}

		timePos = 0;
		duration = animations[currentAnimationIndex]->duration / animations[currentAnimationIndex]->ticksPerSecond;
	}

	//printf("animation speed : %f", animations[currentAnimationIndex]->ticksPerSecond);
}

void ScratchEngine::Animator::UpdateTransforms(Bone * node)
{
	CalculateBoneToWorldTransform(node);
	for (UINT a = 0; a < node->children.size(); a++)
	{
		Bone* child = node->children[a];
		UpdateTransforms(child);
	}
}

bool ScratchEngine::Animator::SetAnimation(string animation)
{
	map<std::string, int>::iterator it = animationNameToId.find(animation);

	if (it != animationNameToId.end() && it->second != currentAnimationIndex)
	{
		SetAnimationIndex(it->second);
		return true;
	}

	return false;
}

//void ScratchEngine::Animator::SetSingleAnimation(int current)
//{
//	if (current >= animations.size())
//		return;
//
//	int oldIndex = currentAnimationIndex;
//	currentAnimationIndex = current;
//	if (oldIndex != currentAnimationIndex) {
//		LoopClips = false;
//		timePos = 0;
//		duration = animations[currentAnimationIndex]->duration / animations[currentAnimationIndex]->ticksPerSecond;
//	}
//}

bool ScratchEngine::Animator::LoadAnimations(const aiScene* scene)
{
	int previous = animations.size();
	int after = ExtractAnimations(scene);

	const float timestep = 1.0f / 30.0f;

	for (UINT i = previous; i < after; i++)
	{
		AnimationClip* animation = animations[i];

		float dt = 0.0f;
		for (float ticks = 0.0f; ticks < animation->duration; ticks += animation->ticksPerSecond / 30.0f)
		{
			Calculate(dt,i);

			vector<XMMATRIX> Ws;
			for (UINT a = 0; a < bones.size(); a++)
			{
				XMMATRIX W = bones[a]->offset * bones[a]->globalTransform;
				Ws.push_back(XMMatrixTranspose(W));
			}

			animation->transforms.push_back(Ws);

			dt += timestep;
		}
	}

	return true;
}

int ScratchEngine::Animator::ExtractAnimations(const aiScene * scene)
{
	for (UINT i = 0; i < scene->mNumAnimations;i++)
		animations.push_back(new AnimationClip(scene->mAnimations[i]));

	for (UINT i = 0; i < animations.size(); i++)
		animationNameToId[animations[i]->name] = i;

	return animations.size();
}

void ScratchEngine::Animator::Update(float dt)
{
	if (blendFactor < 1)
		blendFactor += blendSpeed * dt;
	else
	{
		if (!LoopClips)
		{
			if (timePos >= duration)
				isPlaying = false;
			else
			{
				timePos += dt;
				isPlaying = true;
			}
		}
		else if (LoopClips)
			timePos += dt;
	}
}

void ScratchEngine::Animator::PlayAnimationForward()
{
	isPlaying = true;
	animations[currentAnimationIndex]->playAnimationForward = true;
}

void ScratchEngine::Animator::PlayAnimationBackward()
{
	isPlaying = true;
	animations[currentAnimationIndex]->playAnimationForward = false;
}

void ScratchEngine::Animator::AdjustAnimationSpeedBy(float prc)
{
	animations[currentAnimationIndex]->ticksPerSecond *= prc;
	duration = animations[currentAnimationIndex]->duration / animations[currentAnimationIndex]->ticksPerSecond;
}

void ScratchEngine::Animator::AdjustAnimationSpeedTo(float ticksPerSec)
{
	animations[currentAnimationIndex]->ticksPerSecond = ticksPerSec;
	duration = animations[currentAnimationIndex]->duration / animations[currentAnimationIndex]->ticksPerSecond;
}

vector<XMMATRIX> ScratchEngine::Animator::GetTransforms()
{
	if (blendFactor >= 1 || previousAnimationIndex == null_index)
		return animations[currentAnimationIndex]->GetTransforms(timePos);

	return blender(blendFactor);
}

int ScratchEngine::Animator::GetBoneIndex(string name)
{
	std::map<std::string, int>::iterator it = bonesToIndex.find(name);
	if (it != bonesToIndex.end()){
		return bonesToIndex[name];
	}
	return -1;
}

Bone* ScratchEngine::Animator::CreateBoneTree(aiNode* node, Bone* parent)
{
	Bone* internalNode = new Bone(std::string(node->mName.C_Str()), parent);

	if (internalNode->name == "")
		internalNode->name = "bone" + _i++;
	
	bonesByName[internalNode->name] = internalNode;
	XMMATRIX trans = ToMatrix(node->mTransformation);
	trans = XMMatrixTranspose(trans);

	internalNode->localTransform = trans;

	internalNode->originalLocalTransform = internalNode->localTransform;
	CalculateBoneToWorldTransform(internalNode);
	for (UINT i = 0; i < node->mNumChildren; i++) {
		Bone * child = CreateBoneTree(node->mChildren[i], internalNode);
		if (child != nullptr) {
			internalNode->children.push_back(child);
		}
	}
	return internalNode;
}

void ScratchEngine::Animator::Calculate(float dt , int index)
{
	if ((index == null_index) | (index >= animations.size()))
		return;
	
	animations[index]->Evaluate(dt, bonesByName);
	UpdateTransforms(skeleton);
}

void ScratchEngine::Animator::CalculateBoneToWorldTransform(Bone * child)
{
	child->globalTransform = child->localTransform;
	Bone * Parent = child->parent;
	while (Parent != nullptr) {
		child->globalTransform *= Parent->localTransform;
		Parent = Parent->parent;
	}
}

XMMATRIX ScratchEngine::Animator::ToMatrix(aiMatrix4x4 transform)
{
	return XMMatrixSet(transform.a1, transform.a2, transform.a3, transform.a4,
		transform.b1, transform.b2, transform.b3, transform.b4,
		transform.c1, transform.c2, transform.c3, transform.c4,
		transform.d1, transform.d2, transform.d3, transform.d4);
}
