#pragma once

#include "SimpleShader.h"
#include <WICTextureLoader.h>
#include "ShadowMap.h"

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
				ID3D11SamplerState* _sampler
			);


			SimpleVertexShader* GetVertexShader();
			SimplePixelShader* GetPixelShader();

			ID3D11ShaderResourceView* getTexture() const { return texture; }
			ID3D11ShaderResourceView* getNormalMap() const { return normalMap; }
			ID3D11SamplerState* getSampler() const { return sampler;  }
			ID3D11ShaderResourceView* getMetalnessMap() const { return metalnessMap; }
			ID3D11ShaderResourceView* getRoughnessMap() const { return roughnessMap; }
			ShadowMap* getShadowMap() const { return shadowMap; }


			void setTexture(ID3D11ShaderResourceView* _texture);
			void setNormalMap(ID3D11ShaderResourceView* _normalMap);
			void setShadowMap(ShadowMap* _shadowMap);
			void setMetalnessMap(ID3D11ShaderResourceView* _metalnessMap);
			void setRoughnessMap(ID3D11ShaderResourceView* _roughnessMap);

			int HasTexture() const { return hasTexture; }
			int HasNormalMap() const { return hasNormalMap; }
			int HasShadowMap() const { return hasShadowMap; }
			int HasMetalnessMap() const { return hasMetalnessMap; }
			int HasRoughnessMap() const { return hasRoughnessMap; }

			
		private:
			SimpleVertexShader* vertexShader;
			SimplePixelShader* pixelShader;

			ID3D11ShaderResourceView* texture;
			ID3D11ShaderResourceView* normalMap;
			ShadowMap * shadowMap;
			ID3D11ShaderResourceView* metalnessMap;
			ID3D11ShaderResourceView* roughnessMap;

			int hasTexture;
			int hasNormalMap;
			int hasShadowMap;
			int hasMetalnessMap;
			int hasRoughnessMap;

			ID3D11SamplerState* sampler;
		};
	}
}
