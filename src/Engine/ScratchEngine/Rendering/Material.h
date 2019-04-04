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
				ID3D11SamplerState* _sampler,
				ID3D11SamplerState* _shadowSampler
			);

			SimpleVertexShader* GetVertexShader();
			SimplePixelShader* GetPixelShader();

			ID3D11ShaderResourceView* getTexture();
			ID3D11ShaderResourceView* getNormalMap();
			ID3D11SamplerState* getSampler();
			ID3D11SamplerState* getShadowSampler();

			void setShadowMap(ID3D11ShaderResourceView* _shadowMap);
			
		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			ID3D11ShaderResourceView* texture;
			ID3D11ShaderResourceView* normalMap;
			ID3D11ShaderResourceView* shadowMap;
			ID3D11SamplerState* sampler;
			ID3D11SamplerState* shadowSampler;
		};
	}
}
