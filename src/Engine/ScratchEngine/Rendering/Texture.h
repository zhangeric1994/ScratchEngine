#ifndef TEXTURE_H
#define TEXTURE_H
#pragma once

#include <WICTextureLoader.h>

#include "prerequisites.h"

#include "../Common/Typedefs.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		class __declspec(dllexport) Texture
		{
			friend class RenderingEngine;


		protected:
			ID3D11SamplerState* samplerState;
			ID3D11ShaderResourceView* shaderResourceView;

			u32 width;
			u32 height;


			Texture();


		public:
			Texture(wchar_t* _filepath, D3D11_TEXTURE_ADDRESS_MODE _addressMode, D3D11_FILTER _filterMode, f32 _maxLOD);
			~Texture();
			

			u32 GetWidth() const;
			u32 GetHeight() const;
		};


		class __declspec(dllexport) Shadow : public Texture
		{
			friend class RenderingEngine;


		private:
			LightType type;
			i32 numCascades;

			ID3D11DepthStencilView* depthStencilViews[6];
			ID3D11RenderTargetView* renderTargetViews[6];
			union
			{
				XMMATRIX viewProjection;
			};


		public:
			Shadow(u32 size, LightType type, int numCascades = 1);
			Shadow(u32 width, u32 height, LightType type, int numCascades = 1);
			~Shadow();

			i32 GetNumCascades() const;
		};
	}
}


inline u32 ScratchEngine::Rendering::Texture::GetWidth() const
{
	return this->width;
}

inline u32 ScratchEngine::Rendering::Texture::GetHeight() const
{
	return this->height;
}


inline i32 ScratchEngine::Rendering::Shadow::GetNumCascades() const
{
	return this->numCascades;
}
#endif // !TEXTURE_H