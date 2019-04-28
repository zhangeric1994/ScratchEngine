#ifndef RENDERABLE_H
#define RENDERABLE_H
#pragma once

#include "Material.h"
#include "Mesh.h"

namespace ScratchEngine
{
	namespace Rendering
	{
		struct Renderable
		{
			friend class RenderingEngine;


		private:
			XMMATRIX worldMatrix;

			Mesh* mesh;
			Material* material;
			XMMATRIX bones[MAX_NUM_BONES_PER_MODEL];
			//int boneSize;
			
			Renderable();
		};
	}
}
#endif