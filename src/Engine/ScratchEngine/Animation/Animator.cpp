#include "Animator.h"

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

		LoopClips = true;
		timePos = 0;
		duration = animations[currentAnimationIndex]->duration/animations[currentAnimationIndex]->ticksPerSecond;
	
		blendFactor = 0;
	}

	//printf("animation speed : %f", animations[currentAnimationIndex]->ticksPerSecond);
}

void ScratchEngine::Animator::UpdateTransforms(Bone * node)
{
	CalculateBoneToWorldTransform(node);
	for (UINT a = 0; a < node->children.size(); a++) {
		Bone* child = node->children[a];
		UpdateTransforms(child);
	}
}

bool ScratchEngine::Animator::SetAnimation(string animation)
{
	std::map<std::string, int>::iterator it = animationNameToId.find(animation);

	if (it != animationNameToId.end() && it->second != currentAnimationIndex)
	{
		SetAnimationIndex(it->second);
		return true;
	}

	return false;
}

void ScratchEngine::Animator::SetSingleAnimation(int current)
{
	if (current >= animations.size()) {
		return;
	}

	int oldIndex = currentAnimationIndex;
	currentAnimationIndex = current;
	if (oldIndex != currentAnimationIndex) {
		LoopClips = false;
		timePos = 0;
		duration = animations[currentAnimationIndex]->duration / animations[currentAnimationIndex]->ticksPerSecond;
	}
}

bool ScratchEngine::Animator::LoadAnimations(const aiScene* scene)
{
	int previous = animations.size();
	int after = ExtractAnimations(scene);

	const float timestep = 1.0f / 30.0f;

	for (UINT i = previous; i < after; i++) {
		AnimationClip* animation = animations[i];
		float dt = 0.0f;
		for (float ticks = 0.0f; ticks < animation->duration; ticks += animation->ticksPerSecond / 30.0f) {
			dt += timestep;
			Calculate(dt,i);

			vector<XMMATRIX> Ws;
			for (UINT a = 0; a < bones.size(); a++)
			{
				XMMATRIX W = bones[a]->offset * bones[a]->globalTransform;
				Ws.push_back(XMMatrixTranspose(W));
			}
			animation->transforms.push_back(Ws);
		}
	}
	return true;
}

int ScratchEngine::Animator::ExtractAnimations(const aiScene * scene)
{
	for (UINT i = 0; i < scene->mNumAnimations;i++)
	{
		aiAnimation * animation = scene->mAnimations[i];
		animations.push_back(new AnimationClip(animation));
	}

	for (UINT i = 0; i < animations.size(); i++)
		animationNameToId[animations[i]->name] = i;
	return animations.size();
}

void ScratchEngine::Animator::Update(float dt)
{
	if (blendFactor < 1)
		blendFactor += blendFactor * dt;
	else
		blendFactor = 1;

	isPlaying = false;
	if (!LoopClips && timePos + dt < duration) {
			timePos += dt;
			isPlaying = true;
			//SetAnimationIndex(nextAnimation);
	}
	else if(LoopClips){
			timePos += dt;
			isPlaying = true;
	}
}

void ScratchEngine::Animator::PlayAnimationForward()
{
	animations[currentAnimationIndex]->playAnimationForward = true;
}

void ScratchEngine::Animator::PlayAnimationBackward()
{
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

std::vector<XMMATRIX> ScratchEngine::Animator::GetTransforms()
{
	//if (blendFactor == 1 || previousAnimationIndex == null_index)
		return animations[currentAnimationIndex]->GetTransforms(timePos);

	//vector<XMMATRIX> currentWorldMatrices = animations[currentAnimationIndex]->GetTransforms(timePos);
	//vector<XMMATRIX> previousWorldMatrices = animations[previousAnimationIndex]->GetTransforms(timePos);

	//for (int i = 0; i < currentWorldMatrices.size(); ++i)
	//{
	//	XMMATRIX currentWorldMatrix = XMMatrixTranspose(currentWorldMatrices[i]);
	//	XMMATRIX previousWorldMatrix = XMMatrixTranspose(previousWorldMatrices[i]);

	//	XMVECTOR currentTranslation = currentWorldMatrix.r[0];
	//	XMVECTOR currentRotation = XMQuaternionRotationMatrix(currentWorldMatrix);

	//	XMVECTOR previousTranslation = previousWorldMatrix.r[0];
	//	XMVECTOR previousRotation = XMQuaternionRotationMatrix(previousWorldMatrix);

	//	currentWorldMatrices[i] = XMMatrixTranspose(XMMatrixRotationQuaternion(XMQuaternionSlerp(previousRotation, currentRotation, blendFactor)) * XMMatrixTranslationFromVector(XMVectorLerp(previousTranslation, currentTranslation, blendFactor)));
	//}

	//return currentWorldMatrices;
}

int ScratchEngine::Animator::GetBoneIndex(string name)
{
	std::map<std::string, int>::iterator it = bonesToIndex.find(name);
	if (it != bonesToIndex.end()){
		return bonesToIndex[name];
	}
	return -1;
}

Bone * ScratchEngine::Animator::CreateBoneTree(aiNode * node, Bone *Parent)
{
	Bone * internalNode = new Bone(std::string(node->mName.C_Str()), Parent);

	if (internalNode->name == "") {
		internalNode->name = "foo" + _i++;
	}
	

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
	if ((index == null_index) | (index >= animations.size())) {
		return;
	}
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
