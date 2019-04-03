#pragma once

#include "SimpleShader.h"
#include <WICTextureLoader.h>

namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Material
		{
		public:
			Material(
				SimpleVertexShader* _vertexShader,
				SimplePixelShader* _pixelShader,
				ID3D11ShaderResourceView* _texture,
				ID3D11ShaderResourceView* _normalMap,
				ID3D11SamplerState* _sampler
			);

			SimpleVertexShader* GetVertexShader();
			SimplePixelShader* GetPixelShader();

			ID3D11ShaderResourceView* getTexture();
			ID3D11SamplerState* getSampler();

			bool setTexture(
				ID3D11Device* device,
				ID3D11DeviceContext* context,
				const wchar_t* texturePath
			);

			bool setNormalMap(
				ID3D11Device* device,
				ID3D11DeviceContext* context,
				const wchar_t* texturePath
			);
			
		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			ID3D11ShaderResourceView* texture;
			ID3D11ShaderResourceView* normalMap;
			ID3D11SamplerState* sampler;
		};
	}
}
