#ifndef TEXTURE_H
#define TEXTURE_H
#pragma once

#include "prerequisites.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		class __declspec(dllexport) Texture
		{
			friend class RenderingEngine;


		protected:
			ID3D11ShaderResourceView* shaderResourceView;
			ID3D11SamplerState* samplerState;

			Texture(ID3D11ShaderResourceView* shaderResourceView = nullptr, ID3D11SamplerState* samplerState = nullptr);


		public:
			Texture(const wchar_t* filePath, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_BORDER, D3D11_FILTER filterMode = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, f32 maxLOD = D3D11_FLOAT32_MAX);
			~Texture();
		};


		class __declspec(dllexport) Texture2D : protected Texture
		{
		private:
			union
			{
				u32 size[2];
				struct
				{
					u32 width;
					u32 height;
				};
			};


		public:
			u32 GetWidth() const;
			u32 GetHeight() const;
		};


		class __declspec(dllexport) TextureCube : protected Texture
		{
			friend class RenderingEngine;


		protected:
			u32 size;


		public:
			TextureCube(const wchar_t* filePath, const D3D11_SAMPLER_DESC* samplerDesc);


			u32 GetSize() const;
		};


		class __declspec(dllexport) Shadow : protected Texture
		{
			friend class RenderingEngine;


		private:
			LightType type;
			union
			{
				u32 size[2];
				struct
				{
					u32 width;
					u32 height;
				};
			};
			i32 numCascades;

			ID3D11DepthStencilView* depthStencilViews[6];
			ID3D11RenderTargetView* renderTargetViews[6];


		public:
			Shadow(u32 size, LightType type, int numCascades = 1);
			Shadow(u32 width, u32 height, LightType type, int numCascades = 1);
			~Shadow();

			LightType GetType() const;
			u32 GetWidth() const;
			u32 GetHeight() const;
			i32 GetNumCascades() const;
		};
	}
}


#include "Texture.inl"

#endif // !TEXTURE_H