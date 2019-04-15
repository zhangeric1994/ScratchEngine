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
			const float* GetTint() const;
			ID3D11ShaderResourceView* GetTexture();
			ID3D11SamplerState* GetSampler();

			void SetTint(float r, float g, float b);
			void SetTint(float r, float g, float b, float a);
			void SetTint(const float* color);


		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			float tint[4];

			ID3D11ShaderResourceView* texture;
			ID3D11SamplerState* sampler;
		};
	}
}
