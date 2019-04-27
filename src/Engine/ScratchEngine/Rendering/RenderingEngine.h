#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H
#pragma once

#include <string>

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"
#include "../Memory/DynamicPoolAllocator.hpp"
#include "../Memory/StackAllocator.hpp"
#include "../Memory/PoolAllocator.hpp"

#include "Camera.h"
#include "CubeMap.h"
#include "Light.h"
#include "LightSource.h"
#include "Mesh.h"
#include "Renderable.h"
#include "Renderer.h"
#include "ShadowMap.h"
#include "Texture.h"
#include "Viewer.h"

using namespace ScratchEngine;
using namespace ScratchEngine::Memory;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct RenderingEngineConfig
		{
			ID3D11Device* device = nullptr;
			ID3D11DeviceContext* deviceContext = nullptr;

			i32 maxNumMaterials = DEFAULT_MAX_NUM_MATERIALS;
			i32 maxNumMeshes = DEFAULT_MAX_NUM_MESHES;
			i32 initialNumRenderables = 128;
			i32 initialNumViews = 2;
			i32 initialNumLightSources = 8;
		};


		class RenderingEngine
		{
			friend class Camera;
			friend class Light;
			friend struct Mesh;
			friend class Renderer;
			friend class RenderTexture;
			friend class Texture;


		private:
			static RenderingEngine* singleton;


		public:
			static RenderingEngine* GetSingleton();

			static void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
			static void Initialize(RenderingEngineConfig config);
			static void Terminate();


		private:
			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;

			SimpleVertexShader* vsDepthOnly;
			ID3D11DepthStencilState* dssLessEqual;

			PoolAllocator<sizeof(Material)> materialAllocator;
			PoolAllocator<sizeof(Mesh)> meshAllocator;

			DynamicStackAllocator<Renderable> renderableAllocator;
			DynamicPoolAllocator<Viewer> viewerAllocator;
			DynamicStackAllocator<LightSource> lightSourceAllocator;

			Renderer* rendererList;
			Camera* cameraList;
			Light* lightList;
			ShadowMap* shadow;
			bool hasZPrepass;


			RenderingEngine(RenderingEngineConfig config);
			~RenderingEngine();

			void AddRenderer(Renderer* renderer);
			void AddCamera(Camera* camera);
			void AddLight(Light* light);

			void RemoveRenderer(Renderer* renderer);
			void RemoveCamera(Camera* camera);
			void RemoveLight(Light* light);

			void DestroyRenderable(i32 id);
			void DestroyViewer(i32 id);

			//XMMATRIX shadowView;
			//XMMATRIX shadowProjection;
			XMFLOAT4X4 shadowViewProjectionMat;


		public:
			void UpdateRenderables();
			void UpdateViewers();
			void UpdateLightSources();
			void SortRenderables();
			void PerformZPrepass();
			void DrawForward();
			bool RenderShadowMap();
			void SetShadowMap(ShadowMap* _shadow);
			void RenderCubeMap(CubeMap* cubeMap);
		};


		inline RenderingEngine* RenderingEngine::GetSingleton()
		{
			return singleton;
		}

		inline void RenderingEngine::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
		{
			RenderingEngineConfig config;
			config.device = device;
			config.deviceContext = deviceContext;

			singleton = new RenderingEngine(config);
		}

		inline void RenderingEngine::Initialize(RenderingEngineConfig config)
		{
			singleton = new RenderingEngine(config);
		}

		inline void RenderingEngine::Terminate()
		{
			if (singleton)
				delete singleton;
		}
	}
}
#endif