#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H
#pragma once

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"
#include "../Memory/DynamicPoolAllocator.h"
#include "../Memory/PoolAllocator.h"

#include "Renderer.h"

using namespace ScratchEngine::Memory;

namespace ScratchEngine
{
	namespace Rendering
	{
		class RenderingEngine
		{
			friend class Mesh;
			friend class Renderer;


		private:
			static RenderingEngine* singleton;

			static RenderingEngine* GetSingleton();
			static void Initialize(i32 maxNumMaterials = DEFAULT_MAX_NUM_MATERIALS, i32 maxNumMeshes = DEFAULT_MAX_NUM_MESHES, i32 defaultNumRenderables = 128);

			PoolAllocator<sizeof(Material)> materialAllocator;
			PoolAllocator<sizeof(Mesh)> meshAllocator;
			DynamicPoolAllocator<Renderable> renderableAllocator;

			Renderer* rendererList;

			RenderingEngine(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables);
			~RenderingEngine();

			void AddRenderer(Renderer* renderer);
			i32 CreateRenderable();

			void RemoveRenderer(Renderer* renderer);
			void RemoveRenderable(i32 id);
		};
	}
}
#endif