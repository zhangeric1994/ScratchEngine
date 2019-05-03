#include <WICTextureLoader.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Material.h"
#include "RenderingEngine.h"

using namespace DirectX;


ScratchEngine::Rendering::Material::Material(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, ID3D11SamplerState* _sampler)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;

	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;

	sampler = _sampler;

	diffuseMap = 0;
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

ScratchEngine::Rendering::Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11SamplerState* sampler, const string& filePath)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;

	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;

	this->sampler = sampler;

	diffuseMap = 0;
	normalMap = 0;
	shadowMap = 0;
	metalnessMap = 0;
	roughnessMap = 0;

	hasTexture = 0;
	hasNormalMap = 0;
	hasShadowMap = 0;
	hasMetalnessMap = 0;
	hasRoughnessMap = 0;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_ConvertToLeftHanded);

	if (scene && scene->HasMaterials())
	{
		const aiMaterial* material = scene->mMaterials[0];

		aiTextureType textureType = aiTextureType::aiTextureType_DIFFUSE;

		if (material->GetTextureCount(textureType) > 0)
		{
			aiString path;
			material->GetTexture(textureType, 0, &path);

			string sPath = path.C_Str();

			RenderingEngine* renderingEngine = RenderingEngine::GetSingleton();

			ID3D11ShaderResourceView* srv;

			if (sPath[0] == '*')
			{
				const aiTexture* texture = scene->mTextures[atoi(&sPath[1])];
				CreateWICTextureFromMemory(renderingEngine->device, reinterpret_cast<uint8_t*>(texture->pcData), texture->mWidth, nullptr, &srv);
			}
			else
			{
				sPath = filePath.substr(0, filePath.find_last_of('/') + 1) + sPath;
				CreateWICTextureFromFile(renderingEngine->device, renderingEngine->deviceContext, wstring(sPath.begin(), sPath.end()).c_str(), 0, &srv);
			}

			diffuseMap = srv;
			hasTexture = srv != nullptr;
		}
	}
}


SimpleVertexShader* ScratchEngine::Rendering::Material::GetVertexShader() {
	return vertexShader;
}

SimplePixelShader* ScratchEngine::Rendering::Material::GetPixelShader() {
	return pixelShader;
}


void ScratchEngine::Rendering::Material::setTexture(ID3D11ShaderResourceView * _texture) {
	diffuseMap = _texture;
	if(diffuseMap != 0) hasTexture = 1;
}

void ScratchEngine::Rendering::Material::setNormalMap(ID3D11ShaderResourceView * _normalMap) {
	normalMap = _normalMap;
	if (normalMap != 0) hasNormalMap = 1;
}

void ScratchEngine::Rendering::Material::setShadowMap(ShadowMap* _shadowMap) {
	shadowMap = _shadowMap;
	hasShadowMap = 1;
}

void ScratchEngine::Rendering::Material::setMetalnessMap(ID3D11ShaderResourceView * _metalnessMap) {
	metalnessMap = _metalnessMap;
	if (metalnessMap != 0 )hasMetalnessMap = 1;
}

void ScratchEngine::Rendering::Material::setRoughnessMap(ID3D11ShaderResourceView * _roughnessMap) {
	roughnessMap = _roughnessMap;
	if (roughnessMap != 0) hasRoughnessMap = 1;
}

const float* ScratchEngine::Rendering::Material::GetTint() const
{
	return tint;
}

ID3D11ShaderResourceView* ScratchEngine::Rendering::Material::GetTexture() {
	return diffuseMap;
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
