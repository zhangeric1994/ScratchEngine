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
			const float* GetTint() const;
			ID3D11ShaderResourceView* GetTexture();
			ID3D11SamplerState* GetSampler();

<<<<<<< HEAD
			void SetTint(float r, float g, float b);
			void SetTint(float r, float g, float b, float a);
			void SetTint(const float* color);


=======
>>>>>>> parent of 6cda99c... put metalness map and roughness map into material class. Shadow map occured some unknown bug. It is disabled for now.
			ID3D11ShaderResourceView* getTexture();
			ID3D11ShaderResourceView* getNormalMap();
			ID3D11SamplerState* getSampler();

			void setShadowMap(ID3D11ShaderResourceView* _shadowMap);
			
		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			float tint[4];

			ID3D11ShaderResourceView* texture;
			ID3D11ShaderResourceView* normalMap;
			ID3D11ShaderResourceView* shadowMap;
			ID3D11SamplerState* sampler;
		};
	}
}
