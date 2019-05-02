#pragma once
#include <d3d11.h>
#include <fstream> 
#include <vector>
#include <map>
#include <DirectXMath.h>

#include "Bone.h"
#include "../Core/GameComponent.h"
#include "AnimationClip.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace std;
using namespace ScratchEngine::Animation;

namespace ScratchEngine
{
	struct AnimationBlender
	{
	private:
		vector<XMVECTOR> firstTranslation;
		vector<XMVECTOR> firstRotation;
		vector<XMVECTOR> firstScale;
		vector<XMVECTOR> secondTranslation;
		vector<XMVECTOR> secondRotation;
		vector<XMVECTOR> secondScale;


	public:
		AnimationBlender();
		AnimationBlender(vector<XMMATRIX> first, vector<XMMATRIX> second);

		vector<XMMATRIX> operator()(f32 blendFactor);

		void SetData(vector<XMMATRIX> first, vector<XMMATRIX> second);
	};

	class __declspec(dllexport) Animator : public GameComponent
	{
	protected:
		int _i;
		Bone* skeleton;
		map<string, Bone*> bonesByName;
		map<string, int> bonesToIndex;
		map<string, int> animationNameToId;
		vector<Bone*> bones;
		vector<AnimationClip *> animations;

		void CalculateBoneToWorldTransform(Bone* child);
		void UpdateTransforms(Bone* node); 
		int ExtractAnimations(const aiScene* scene);
		
		void  Calculate(float dt, int index);
		Bone* CreateBoneTree(aiNode* rootNode, Bone* Parent);
		XMMATRIX ToMatrix(aiMatrix4x4 transform);
	//private:
	//	Animator* next;
	//	Animator* previous;

	public:
		Animator();
		Animator(const aiScene* scene);
		~Animator();

		// current playing animation stats
		float timePos;
		bool LoopClips;

		int previousAnimationIndex;
		int currentAnimationIndex;
		
		bool hasSkeleton;
		bool isPlaying;
		string animationName;
		float animationSpeed;
		float duration;
		
		float blendFactor;
		float blendSpeed;

		Animator* next;
		Animator* previous;

		AnimationBlender blender;

		void Update(float dt);
		bool SetAnimation(string animation, bool loop);
		//void SetSingleAnimation(int current);
		float SetAnimationIndex(int animIndex,bool loop);
		void PlayAnimationForward();
		void PlayAnimationBackward();
		void AdjustAnimationSpeedBy(float prc);
		void AdjustAnimationSpeedTo(float ticksPerSec);
		std::vector<XMMATRIX> GetTransforms();
		int GetBoneIndex(string name);

		bool LoadAnimations(const aiScene* scene);
	};
}