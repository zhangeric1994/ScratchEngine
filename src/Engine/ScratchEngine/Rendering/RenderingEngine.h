#ifndef RENDERING_ENGINE_H
#define RENDERING_ENGINE_H
#pragma once

#include <DirectXMath.h>
#include <string>

#include "../Common/Settings.h"
#include "../Common/Typedefs.h"
#include "../Memory/PoolAllocator.hpp"

#include "prerequisites.h"

#include "GFSDK_SSAO.h"
#include "Material.h"
#include "Mesh.h"

using namespace DirectX;
using namespace ScratchEngine;
using namespace ScratchEngine::Memory;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct RenderingEngineConfig
		{
			ID3D11Device2* device = nullptr;
			ID3D11DeviceContext2* deviceContext = nullptr;

			i32 maxNumMaterials = DEFAULT_MAX_NUM_MATERIALS;
			i32 maxNumMeshes = DEFAULT_MAX_NUM_MESHES;
		};


		class RenderingEngine
		{
			friend struct Material;
			friend struct Mesh;

			friend class Shadow;
			friend class Texture;


		private:
			static RenderingEngine* singleton;


		public:
			static RenderingEngine* GetSingleton();

			static void Initialize(ID3D11Device2* device, ID3D11DeviceContext2* deviceContext);
			static void Initialize(RenderingEngineConfig config);
			static void Terminate();


		private:
			ID3D11Device2* device;
			ID3D11DeviceContext2* deviceContext;

			GFSDK_SSAO_Context_D3D11* ssaoContext;

			SimpleVertexShader* vsDepthOnly;
			SimpleVertexShader* vsPositionOnly;
			SimpleVertexShader* vsDirectionalLight;
			SimpleVertexShader* vsPointLight;
			SimpleVertexShader* vsPointLightShadow;
			SimpleVertexShader* vsViewport;
			SimplePixelShader* psSolidColor;
			SimplePixelShader* psGBuffer;
			SimplePixelShader* psDirectionalLight;
			SimplePixelShader* psPointLight;
			SimplePixelShader* psPointLightShadow;
			SimplePixelShader* psAmbientLight;
			SimplePixelShader* psDeferred;
			SimplePixelShader* psShadowVolume;

			ID3D11RasterizerState* rsInsideOut;
			ID3D11RasterizerState* rsShadow;
			ID3D11RasterizerState* rsWireframe;

			ID3D11DepthStencilState* dssLessEqual;
			ID3D11DepthStencilState* dsReadGreater;
			ID3D11DepthStencilState* dsOff;
			ID3D11DepthStencilState* dssCSM;
			
			ID3D11BlendState* bsAdditive;

			ID3D11SamplerState* shadowSampler;

			Mesh* sphereMesh;
			Mesh* cubeMesh;

			PoolAllocator<sizeof(Material)> materialAllocator;
			PoolAllocator<sizeof(Mesh)> meshAllocator;

			ShadowMap* shadow;
			bool hasZPrepass;

			XMFLOAT4X4 shadowViewProjectionMat;


			RenderingEngine(RenderingEngineConfig config);
			~RenderingEngine();


		public:
			void PerformZPrepass(Viewer* viewer, Renderable* renderables, int numRenderables);
			void DrawForward(Viewer* viewer, Renderable* renderables, int numRenderables, LightSource* lightSources, int numLightSources);
			void DrawGBuffers(Viewer* viewer, Renderable* renderables, int numRenderables, ID3D11RenderTargetView*const* gBuffers, int numGBuffers, ID3D11DepthStencilView* depthStencilView);
			void DrawLightBuffer(Viewer* viewer, LightSource* lightSources, int numLightSources, ID3D11ShaderResourceView** gBuffers, ID3D11RenderTargetView* lightBuffer, ID3D11DepthStencilView* depthStencilView, ID3D11ShaderResourceView* stencilSRV);
			void DrawDeferred(ID3D11ShaderResourceView* lightBuffer, ID3D11RenderTargetView* backBuffer, ID3D11DepthStencilView* depthStencilView);
			void RenderSSAO(LightSource* light, ID3D11ShaderResourceView* depthBuffer, ID3D11ShaderResourceView* normalBuffer);
			void RenderSSAO(ID3D11RenderTargetView* ssaoBuffer, XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* depthBuffer, ID3D11ShaderResourceView* normalBuffer);
			void RenderShadowMap(LightSource* light, Renderable* renderables, int numRenderables);
			void RenderCSM(const CSMConfig& config, Renderable* renderables, int numRenderables);
			void SetShadowMap(ShadowMap* _shadow);
			void RenderCubeMap(CubeMap* cubeMap, Viewer* viewer);

			void RenderCSMDebug(const CSMConfig& config, Renderable* renderables, int numRenderables, ID3D11RenderTargetView* debugView);
			void DrawCSMIndices(Viewer* viewer, LightSource* lightSources, int numLightSources, ID3D11ShaderResourceView** gBuffers, ID3D11RenderTargetView* lightBuffer, ID3D11DepthStencilView* depthStencilView, ID3D11ShaderResourceView* stencilSRV);
		};


		inline RenderingEngine* RenderingEngine::GetSingleton()
		{
			return singleton;
		}

		inline void RenderingEngine::Initialize(ID3D11Device2* device, ID3D11DeviceContext2* deviceContext)
		{
			RenderingEngineConfig config;
			config.device = device;
			config.deviceContext = deviceContext;

			singleton = new RenderingEngine(config);


			singleton->sphereMesh = new Mesh(device, (char*)"../Assets/Models/sphere.obj");
			singleton->cubeMesh = new Mesh(device, (char*)"../Assets/Models/cube.obj");
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