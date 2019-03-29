#pragma once
#include "SimpleShader.h"
#include <WICTextureLoader.h>

class Material {
public:
	Material(
		SimpleVertexShader* _vertexShader,
		SimplePixelShader* _pixelShader,
		ID3D11ShaderResourceView* _texture,
		ID3D11ShaderResourceView* _normalMap,
		ID3D11SamplerState* _sampler
	);

	SimpleVertexShader* getVertexShader();
	SimplePixelShader* getPixelShader();

	void setVertexShader(SimpleVertexShader* _vertexShader);
	void setPixelShader(SimplePixelShader* _pixelShader);

	ID3D11ShaderResourceView* getTexture();
	ID3D11ShaderResourceView* getNormalMap();
	ID3D11SamplerState* getSampler();

private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* normalMap;
	ID3D11SamplerState* sampler;
};