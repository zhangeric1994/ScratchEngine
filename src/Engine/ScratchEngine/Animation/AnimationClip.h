#pragma once

#include "../header.h"

#include "AnimationChannel.h"
#include "Bone.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


namespace ScratchEngine
{
	namespace Animation
	{
		struct __declspec(dllexport) AnimationClip
		{
		public:
			std::string name;
			float duration;
			std::vector<AnimationChannel *> channels;
			bool playAnimationForward;
			float lastTime;
			float ticksPerSecond;
			bool loop;
			std::vector<std::vector<XMMATRIX>> transforms;
			std::vector<XMFLOAT3> lastPositions;
			AnimationClip(aiAnimation * anim);
			std::vector<XMMATRIX> GetTransforms(float dt);
			int GetFrameIndexAt(float dt);
			void Evaluate(float dt, std::map<std::string,Bone*> bonesByName);
			XMFLOAT3 TOXMFLOAT3(aiVector3D val);
			XMFLOAT4 TOXMFLOAT4(aiQuaternion val);
			XMMATRIX TOMATRIX(aiQuaternion val);
		};
	}
}