#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <fstream>
#include <map>
#include <tuple>
#include <vector>

#include "AnimationClip.h"
#include "Bone.h"

#include "../Core/GameComponent.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace std;
using namespace ScratchEngine::Animation;


namespace ScratchEngine
{
	class Renderer;


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
		friend class Scene;


	protected:
		int _i;
		Bone* skeleton;
		map<string, Bone*> bonesByName;
		map<string, int> bonesToIndex;
		map<string, int> animationNameToId;
		vector<Bone*> bones;
		vector<AnimationClip*> animations;

		vector<pair<i32, GameObject*>> slots;

		void CalculateBoneToWorldTransform(Bone* child);
		void UpdateTransforms(Bone* node); 
		int ExtractAnimations(const aiScene* scene);
		
		void  Calculate(float dt, int index);
		Bone* CreateBoneTree(aiNode* rootNode, Bone* Parent);
		XMMATRIX ToMatrix(aiMatrix4x4 transform);


	public:
		Animator();
		Animator(const aiScene* scene);
		~Animator();

		// current playing animation stats
		float timePos;
		bool LoopClips;

		int previousAnimationIndex;
		int currentAnimationIndex;
		
		bool useBlending = false;
		bool hasSkeleton;
		bool isPlaying;
		string animationName;
		float animationSpeed;
		float duration;
		
		float blendFactor;
		float blendSpeed;

		AnimationBlender blender;

		Animator* next;
		Animator* previous;

		void Update(float dt, GameObject* parent);
		bool SetAnimation(string animation, bool loop);
		//void SetSingleAnimation(int current);
		float SetAnimationIndex(int animIndex, bool loop);
		void PlayAnimationForward();
		void PlayAnimationBackward();
		void AdjustAnimationSpeedBy(float prc);
		void AdjustAnimationSpeedTo(float ticksPerSec);
		std::vector<XMMATRIX> GetTransforms();
		int GetBoneIndex(string name);

		bool LoadAnimations(const aiScene* scene);

		void BindToSlot(i32 boneIndex, GameObject* gameObject);
		void UnbindFromSlot(GameObject* gameObject);
	};
}