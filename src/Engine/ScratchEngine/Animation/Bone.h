#pragma once

#include <d3d11_2.h>
#include <DirectXMath.h>
#include <fstream> 
#include <vector>

using namespace DirectX;

namespace ScratchEngine
{
	namespace Animation
	{
		struct __declspec(dllexport) Bone
		{
		public:
			Bone();
			Bone(std::string Name,Bone * parent);

			std::string name;
				// Bind space transform
			XMMATRIX offset;
				// local matrix transform
			XMMATRIX localTransform;
				// To-root transform
			XMMATRIX globalTransform;
				// copy of the original local transform
			XMMATRIX originalLocalTransform;
				// parent bone reference
			Bone * parent;
				// child bone references
			std::vector<Bone*> children;
		};
	}
}