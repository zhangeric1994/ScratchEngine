#pragma once
#include <d3d11.h>
#include "SimpleShader.h"

namespace ScratchEngine {

	namespace Rendering {
		struct __declspec(dllexport) ShadowMap {
		public:
			ShadowMap();
			~ShadowMap();

			void setUp(ID3D11Device* device);
			void setShader(SimpleVertexShader* _shadowShader);
			//void setViewport(float height, float width);

			ID3D11DepthStencilView*		getShadowDSV();
			//D3D11_VIEWPORT				getViewport();
			ID3D11RasterizerState*		getRasterizerState();
			ID3D11ShaderResourceView*	getShadowSRV();
			SimpleVertexShader*			getShadowShader();
			ID3D11SamplerState*			getSampler();

		private:
			D3D11_TEXTURE2D_DESC				shadowDesc;

			D3D11_DEPTH_STENCIL_VIEW_DESC		depthStencilViewDesc;

			D3D11_SHADER_RESOURCE_VIEW_DESC		shaderResourceViewDesc;

			D3D11_RASTERIZER_DESC				shadowRenderStateDesc;

			D3D11_SAMPLER_DESC					shadowSampDesc;

			ID3D11Texture2D*					shadowTexture;

			ID3D11SamplerState*					shadowSampler;

			SimpleVertexShader*					shadowShader;

			ID3D11DepthStencilView*				shadowDepthStencilView;

			ID3D11ShaderResourceView*			shadowResourceView;

			ID3D11RasterizerState*				shadowRasterizerState;

			//may define viewport in game.h
			//D3D11_VIEWPORT						shadowViewport;
		};
	}
}