#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once

#include "prerequisites.h"

using namespace DirectX;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct Renderable
		{
			XMMATRIX worldMatrix;
			Mesh* mesh;
			Material* material;
			XMMATRIX bones[MAX_NUM_BONES_PER_MODEL];
		};
	}
}

#endif