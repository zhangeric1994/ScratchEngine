#include "Material.h"

ScratchEngine::Rendering::Material::Material(
	SimpleVertexShader* _vertexShader,
	SimplePixelShader* _pixelShader,
	ID3D11ShaderResourceView* _texture,
	ID3D11SamplerState* _sampler
) {
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;
	texture = _texture;
	sampler = _sampler;
}

SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() {
	return pixelShader;
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
