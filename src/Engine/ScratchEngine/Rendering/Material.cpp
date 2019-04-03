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

bool ScratchEngine::Rendering::Material::setTexture(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	const wchar_t* texturePath
) {
	return DirectX::CreateWICTextureFromFile(device, context, texturePath, 0, &texture);
}

bool ScratchEngine::Rendering::Material::setNormalMap(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	const wchar_t* texturePath
) {
	return DirectX::CreateWICTextureFromFile(device, context, texturePath, 0, &normalMap);
}