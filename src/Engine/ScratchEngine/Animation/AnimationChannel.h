#pragma once

#include <d3d11_2.h>
#include <fstream> 
#include <vector>
#include <map>
#include <DirectXMath.h>


#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
using namespace DirectX;
namespace ScratchEngine
{
	namespace Animation
	{
		struct __declspec(dllexport) AnimationChannel
		{
		public:
			std::string name;
			std::vector<aiVectorKey> positionKeys;
			std::vector<aiQuatKey> rotationKeys;
			std::vector<aiVectorKey> scalingKeys;

			AnimationChannel(
				std::string name,
				aiVectorKey * _PositionKeys,
				UINT _PosSize,
				aiQuatKey * _RotationKeys,
				UINT _RotSize,
				aiVectorKey * _ScalingKeys,
				UINT _ScaSize
			);
		};
	}
}