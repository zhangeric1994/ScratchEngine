#pragma once

#include "../header.h"


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