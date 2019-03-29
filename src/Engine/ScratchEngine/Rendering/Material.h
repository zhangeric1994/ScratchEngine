#pragma once

#include "SimpleShader.h"

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
				ID3D11SamplerState* _sampler
			);

			SimpleVertexShader* GetVertexShader();
			SimplePixelShader* GetPixelShader();

			ID3D11ShaderResourceView* getTexture();
			ID3D11SamplerState* getSampler();

		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			ID3D11ShaderResourceView* texture;
			ID3D11SamplerState* sampler;
		};
	}
}
