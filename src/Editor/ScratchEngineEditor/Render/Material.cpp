#include "Material.h"

Material::Material(
	SimpleVertexShader* _vertexShader,
	SimplePixelShader* _pixelShader,
	ID3D11ShaderResourceView* _texture,
	ID3D11ShaderResourceView* _normalMap,
	ID3D11SamplerState* _sampler
) {
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	texture = _texture;
	normalMap = _normalMap;
	sampler = _sampler;
}

SimpleVertexShader* Material::getVertexShader() {
	return vertexShader;
}

SimplePixelShader* Material::getPixelShader() {
	return pixelShader;
}

ID3D11ShaderResourceView* Material::getTexture() {
	return texture;
}

ID3D11ShaderResourceView* Material::getNormalMap() {
	return normalMap;
}

ID3D11SamplerState* Material::getSampler() {
	return sampler;
}