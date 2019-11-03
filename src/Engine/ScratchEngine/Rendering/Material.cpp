#include <WICTextureLoader.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Material.h"

#include "RenderingEngine.h"

using namespace DirectX;


ScratchEngine::Rendering::Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11SamplerState* sampler)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;

	tint[0] = 1;
	tint[1] = 1;
	tint[2] = 1;
	tint[3] = 1;

	this->sampler = sampler;

	diffuseMap = nullptr;
	normalMap = nullptr;
	metalnessMap = nullptr;
	roughnessMap = nullptr;
}

ScratchEngine::Rendering::Material::Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11SamplerState* sampler, const string& filePath) : Material(vertexShader, pixelShader, sampler)
{
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
		}
	}
}