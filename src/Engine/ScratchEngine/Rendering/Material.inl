#ifndef MATERIAL_INL
#define MATERIAL_INL
#pragma once

#include "prerequisites.h"


inline SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() const
{
	return vertexShader;
}

inline SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() const
{
	return pixelShader;
}

inline const f32* ScratchEngine::Rendering::Material::GetTint() const
{
	return tint;
}

inline bool ScratchEngine::Rendering::Material::HasTexture() const
{
	return diffuseMap != nullptr;
}

inline bool ScratchEngine::Rendering::Material::HasNormalMap() const
{
	return normalMap != nullptr;
}

inline bool ScratchEngine::Rendering::Material::HasMetalnessMap() const
{
	return metalnessMap != nullptr;
}

inline bool ScratchEngine::Rendering::Material::HasRoughnessMap() const
{
	return roughnessMap != nullptr;
}

inline ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetTexture() const
{
	return diffuseMap;
}

inline ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetNormalMap() const
{
	return normalMap;
}

inline ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetMetalnessMap() const
{
	return metalnessMap;
}

inline ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetRoughnessMap() const
{
	return roughnessMap;
}

inline ID3D11SamplerState* ScratchEngine::Rendering::Material::GetSampler() const
{
	return sampler;
}

inline void ScratchEngine::Rendering::Material::SetTint(f32 r, f32 g, f32 b, f32 a)
{
	tint[0] = r;
	tint[1] = g;
	tint[2] = b;
	tint[3] = a;
}

inline void ScratchEngine::Rendering::Material::SetTint(f32 r, f32 g, f32 b)
{
	tint[0] = r;
	tint[1] = g;
	tint[2] = b;
}

inline void ScratchEngine::Rendering::Material::SetTint(const f32* color)
{
	memcpy(tint, color, 16);
}

inline void ScratchEngine::Rendering::Material::SetTexture(ID3D11ShaderResourceView* texture)
{
	diffuseMap = texture;
}

inline void ScratchEngine::Rendering::Material::SetNormalMap(ID3D11ShaderResourceView* texture)
{
	normalMap = texture;
}

inline void ScratchEngine::Rendering::Material::SetMetalnessMap(ID3D11ShaderResourceView* texture)
{
	metalnessMap = texture;
}

inline void ScratchEngine::Rendering::Material::SetRoughnessMap(ID3D11ShaderResourceView* texture)
{
	roughnessMap = texture;
}

#endif // !MATERIAL_INL