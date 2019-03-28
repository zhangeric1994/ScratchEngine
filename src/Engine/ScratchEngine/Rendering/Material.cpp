#include "Material.h"

ScratchEngine::Rendering::Material::Material(
	SimpleVertexShader* _vertexShader,
	SimplePixelShader* _pixelShader,
	ID3D11ShaderResourceView* _texture,
	ID3D11SamplerState* _sampler
) {
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	texture = _texture;
	sampler = _sampler;
}

SimpleVertexShader* ScratchEngine::Rendering::Material::getVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::getPixelShader() {
	return pixelShader;
}

ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::getTexture() {
	return texture;
}

ID3D11SamplerState* ScratchEngine::Rendering::Material::getSampler() {
	return sampler;
}