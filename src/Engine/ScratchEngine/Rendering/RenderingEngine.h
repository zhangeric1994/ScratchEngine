#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H
#pragma once

#include <string>

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"
#include "../Memory/PoolAllocator.hpp"

#include "CubeMap.h"
#include "LightSource.h"
#include "Mesh.h"
#include "Renderable.h"
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
		};


		class RenderingEngine
		{
			friend struct Mesh;
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

			ShadowMap* shadow;
			bool hasZPrepass;

			//XMMATRIX shadowView;
			//XMMATRIX shadowProjection;
			XMFLOAT4X4 shadowViewProjectionMat;


			RenderingEngine(RenderingEngineConfig config);
			~RenderingEngine();


		public:
			void PerformZPrepass(Viewer* viewer, Renderable* renderables, int numRenderables);
			void DrawForward(Viewer* viewer, Renderable* renderables, int numRenderables, LightSource* lightSources, int numLightSources);
			bool RenderShadowMap(Renderable* renderables, int numRenderables);
			void SetShadowMap(ShadowMap* _shadow);
			void RenderCubeMap(CubeMap* cubeMap, Viewer* viewer);
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
			if (!singleton)
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