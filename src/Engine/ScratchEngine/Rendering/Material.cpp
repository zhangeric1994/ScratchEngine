#include "Material.h"

ScratchEngine::Rendering::Material::Material(
	SimpleVertexShader* _vertexShader,
	SimplePixelShader* _pixelShader,
	ID3D11SamplerState* _sampler
) {
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	sampler = _sampler;

	texture = 0;
	normalMap = 0;
	shadowMap = 0;
	metalnessMap = 0;
	roughnessMap = 0;

	hasTexture = 0;
	hasNormalMap = 0;
	hasShadowMap = 0;
	hasMetalnessMap = 0;
	hasRoughnessMap = 0;
}


SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() {
	return pixelShader;
}

void ScratchEngine::Rendering::Material::setTexture(ID3D11ShaderResourceView * _texture) {
	texture = _texture;
	hasTexture = 1;
}

void ScratchEngine::Rendering::Material::setNormalMap(ID3D11ShaderResourceView * _normalMap) {
	normalMap = _normalMap;
	hasNormalMap = 1;
}

void ScratchEngine::Rendering::Material::setShadowMap(ShadowMap* _shadowMap) {
	shadowMap = _shadowMap;
	hasShadowMap = 1;
}

void ScratchEngine::Rendering::Material::setMetalnessMap(ID3D11ShaderResourceView * _metalnessMap) {
	metalnessMap = _metalnessMap;
	hasMetalnessMap = 1;
}

void ScratchEngine::Rendering::Material::setRoughnessMap(ID3D11ShaderResourceView * _roughnessMap) {
	roughnessMap = _roughnessMap;
	hasRoughnessMap = 1;
}
