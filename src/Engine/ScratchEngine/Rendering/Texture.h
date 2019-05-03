#ifndef TEXTURE_H
#define TEXTURE_H
#pragma once

#include <WICTextureLoader.h>

#include "../Common/Typedefs.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		class __declspec(dllexport) Texture
		{
		protected:
			ID3D11SamplerState* samplerState;
			ID3D11ShaderResourceView* shaderResourceView;

			Texture();


		public:
			Texture(wchar_t* _filepath, D3D11_TEXTURE_ADDRESS_MODE _addressMode, D3D11_FILTER _filterMode, f32 _maxLOD);
			~Texture();


		private:
			ID3D11ShaderResourceView* GetShaderResourceView();
			ID3D11SamplerState* GetSamplerState();
		};


		class __declspec(dllexport) RenderTexture : public Texture
		{
		private:
			ID3D11RenderTargetView* renderTargetView;
			ID3D11DepthStencilView* depthStencilView;
			
			u32 width;
			u32 height;


		public:
			RenderTexture(u32 size);
			RenderTexture(u32 width, u32 height);
			~RenderTexture();


		private:
			ID3D11RenderTargetView* GetRenderTargetView();
			ID3D11DepthStencilView* GetDepthStencilView();
		};


		inline ID3D11ShaderResourceView* ScratchEngine::Rendering::Texture::GetShaderResourceView()
		{
			return shaderResourceView;
		}

		inline ID3D11SamplerState* ScratchEngine::Rendering::Texture::GetSamplerState()
		{
			return samplerState;
		}


		inline ID3D11RenderTargetView* ScratchEngine::Rendering::RenderTexture::GetRenderTargetView()
		{
			return renderTargetView;
		}

		inline ID3D11DepthStencilView* ScratchEngine::Rendering::RenderTexture::GetDepthStencilView()
		{
			return depthStencilView;
		}
	}
}
#endif // !TEXTURE_H