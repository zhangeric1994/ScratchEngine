#pragma once
#include <d3d11.h>
#include "SimpleShader.h"

namespace ScratchEngine {

	namespace Rendering {
		struct __declspec(dllexport) ShadowMap {
		public:
			ShadowMap();
			~ShadowMap();



			SimpleVertexShader* getShadowShader();


		private:
			ID3D11Texture2D*     shadowMap;
			D3D11_TEXTURE2D_DESC shadowDesc;

			ID3D11SamplerState* shadowSampler;

			SimpleVertexShader* shadowShader;

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

			ID3D11DepthStencilView* shadowDepthStencilView;

			ID3D11ShaderResourceView* shadowResourceView;

			D3D11_RASTERIZER_DESC shadowRenderStateDesc;

			ID3D11RasterizerState* shadowRasterizerState;

			//may define viewport in game.h
			D3D11_VIEWPORT shadowViewport;
		};
	}
}