#include "Material.h"

ScratchEngine::Rendering::Material::Material(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, ID3D11SamplerState* _sampler)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;

	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;

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
