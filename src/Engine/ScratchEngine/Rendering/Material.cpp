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
<<<<<<< HEAD

	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;
	texture = _texture;

=======
	texture = _texture;
	normalMap = _normalMap;
>>>>>>> parent of 6cda99c... put metalness map and roughness map into material class. Shadow map occured some unknown bug. It is disabled for now.
	sampler = _sampler;
}

SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() {
	return pixelShader;
}

<<<<<<< HEAD

void ScratchEngine::Rendering::Material::setTexture(ID3D11ShaderResourceView * _texture) {
	texture = _texture;
	hasTexture = 1;
=======
ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::getTexture() {
	return texture;
>>>>>>> parent of 6cda99c... put metalness map and roughness map into material class. Shadow map occured some unknown bug. It is disabled for now.
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

const float* ScratchEngine::Rendering::Material::GetTint() const
{
	return tint;
}

ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetTexture() {
	return texture;
}

ID3D11SamplerState* ScratchEngine::Rendering::Material::GetSampler() {
	return sampler;
}

__inline void ScratchEngine::Rendering::Material::SetTint(float r, float g, float b)
{
	SetTint(r, g, b, 1);
}

__inline void ScratchEngine::Rendering::Material::SetTint(float r, float g, float b, float a)
{
	tint[0] = r;
	tint[1] = g;
	tint[2] = b;
	tint[3] = a;
}

__inline void ScratchEngine::Rendering::Material::SetTint(const float* color)
{
	memcpy(tint, color, 16);
}
