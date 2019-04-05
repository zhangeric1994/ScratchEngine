#include "Material.h"

ScratchEngine::Rendering::Material::Material(
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

SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() {
	return pixelShader;
}

ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::getTexture() {
	return texture;
}

ID3D11SamplerState* ScratchEngine::Rendering::Material::getSampler() {
	return sampler;
}

ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::getNormalMap() {
	return normalMap;
}

void ScratchEngine::Rendering::Material::setShadowMap(ID3D11ShaderResourceView* _shadowMap) {
	shadowMap = _shadowMap;
}
