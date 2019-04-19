#include "Animator.h"

ScratchEngine::Animator::Animator()
{
	timePos = 0.0f;
	skeleton = nullptr;
	LoopClips = true;
	currentAnimationIndex = -1;
	hasSkeleton = false;
}

ScratchEngine::Animator::Animator(const aiScene * scene)
{	
	timePos = 0.0f;
	LoopClips = true;
	if (!scene->HasAnimations()) {
		// no animation
		skeleton = nullptr;
		currentAnimationIndex = -1;
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
	ExtractAnimations(scene);

	const float timestep = 1.0f / 30.0f;

	for (UINT i = 0; i < animations.size(); i++) {
		SetAnimationIndex(i);
		float dt = 0.0f;
		for (float ticks = 0.0f; ticks < animations[i]->duration; ticks += animations[i]->ticksPerSecond / 30.0f) {
			dt += timestep;
			Calculate(dt);
			std::vector<XMMATRIX> trans;
			for (UINT a = 0; a < bones.size(); a++) {
				XMMATRIX rotMat = bones[a]->offset * bones[a]->globalTransform;
				trans.push_back(rotMat);
			}
			animations[i]->transforms.push_back(trans);
		}
	}
	printf("Finished loading animations with %d bones", bones.size());
}

ScratchEngine::Animator::~Animator()
{
	delete skeleton;
}


void ScratchEngine::Animator::SetAnimationIndex(int animIndex)
{
	if (animIndex >= animations.size()) {
		return;
	}
	int oldIndex = currentAnimationIndex;
	currentAnimationIndex = animIndex;
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
	int index;
	std::map<std::string, int>::iterator it = animationNameToId.find(animation);
	if (it != animationNameToId.end()) {
		int oldIndex = currentAnimationIndex;
		currentAnimationIndex = it->second;
		return oldIndex != currentAnimationIndex;
	}
	return false;
}

void ScratchEngine::Animator::ExtractAnimations(const aiScene * scene)
{
	for (UINT i = 0; i < scene->mNumAnimations;i++) {
		aiAnimation * animation = scene->mAnimations[i];
		animations.push_back(new AnimationClip(animation));
	}

	for (UINT i = 0; i < animations.size(); i++) {
		animationNameToId[animations[i]->name] = i;
	}
	currentAnimationIndex = 0;
}

void ScratchEngine::Animator::Update(float dt)
{
	timePos += dt;

	if (timePos > duration) {
		if (LoopClips) {
			timePos = 0;
		}
		else {
			//wait 
			timePos = duration;
		}
	}else{
		// ok state
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
}

void ScratchEngine::Animator::AdjustAnimationSpeedTo(float ticksPerSec)
{
	animations[currentAnimationIndex]->ticksPerSecond = ticksPerSec;
}

std::vector<XMMATRIX> ScratchEngine::Animator::GetTransforms()
{
	return animations[currentAnimationIndex]->GetTransforms(timePos);
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

void ScratchEngine::Animator::Calculate(float dt)
{
	if ((currentAnimationIndex < 0) | (currentAnimationIndex >= animations.size())) {
		return;
	}
	animations[currentAnimationIndex]->Evaluate(dt, bonesByName);
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
