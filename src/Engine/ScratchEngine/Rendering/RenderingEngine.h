#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H
#pragma once

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"
#include "../Memory/DynamicPoolAllocator.hpp"
#include "../Memory/StackAllocator.hpp"
#include "../Memory/PoolAllocator.hpp"

#include "Camera.h"
#include "Light.h"
#include "LightSource.h"
#include "Mesh.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Viewer.h"
#include "ShadowMap.h"
#include "CubeMap.h"

using namespace ScratchEngine;
using namespace ScratchEngine::Memory;

namespace ScratchEngine
{
	namespace Rendering
	{
		class RenderingEngine
		{
			friend class Camera;
			friend class Light;
			friend struct Mesh;
			friend class Renderer;


		private:
			static RenderingEngine* singleton;

			ShadowMap* shadow;

			PoolAllocator<sizeof(Material)> materialAllocator;
			PoolAllocator<sizeof(Mesh)> meshAllocator;
			DynamicStackAllocator<Renderable> renderableAllocator;
			DynamicPoolAllocator<Viewer> viewerAllocator;
			DynamicStackAllocator<LightSource> lightSourceAllocator;
			Renderer* rendererList;
			Camera* cameraList;
			Light* lightList;
			//SimpleVertexShader* vsZPrepass;
			//ID3D11DepthStencilState* zPrepassDepthState;

			RenderingEngine(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables, i32 defaultNumCameraProxies);
			~RenderingEngine();

			void AddRenderer(Renderer* renderer);
			void AddCamera(Camera* camera);
			void AddLight(Light* light);

			void RemoveRenderer(Renderer* renderer);
			void RemoveCamera(Camera* camera);
			void RemoveLight(Light* light);
			void DestroyRenderable(i32 id);
			void DestroyViewer(i32 id);

		public:
			static RenderingEngine* GetSingleton();

			static void Initialize(i32 maxNumMaterials = DEFAULT_MAX_NUM_MATERIALS, i32 maxNumMeshes = DEFAULT_MAX_NUM_MESHES, i32 defaultNumRenderables = 128, i32 defaultNumViews = 4);
			static void Stop();

			void UpdateRenderables();
			void UpdateViewers();
			void UpdateLightSources();
			void SortRenderables();
			void PerformZPrepass(SimpleVertexShader* shader, ID3D11DeviceContext* context);
			void DrawForward(ID3D11DeviceContext* context);
			void RenderShadowMap(ID3D11DeviceContext* context);
			void SetShadowMap(ShadowMap* _shadow);
			void RenderCubeMap(ID3D11DeviceContext* context, CubeMap* cubeMap);
		};
	}
}
#endif