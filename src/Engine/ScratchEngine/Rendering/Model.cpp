#include "Model.h"
using namespace ScratchEngine::Rendering;
ScratchEngine::Rendering::Model::Model(ID3D11Device * device)
{
	this->device = device;
}
ScratchEngine::Rendering::Model::Model(ID3D11Device * device, const std::string & filePath)
{
	this->device = device;
	this->LoadModel(filePath);
	
}

ScratchEngine::Rendering::Model::~Model()
{
}

bool ScratchEngine::Rendering::Model::LoadModel(const std::string & filePath)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate|
		aiProcess_ConvertToLeftHanded
	);
	if (pScene == NULL)
		return false;
	this->ProcessNode(pScene->mRootNode, pScene);
	return true;
}

void ScratchEngine::Rendering::Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshs.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		Model* child = new Model(device);
		child->ProcessNode(node->mChildren[i], scene);
		childModels.push_back(child);
	}
}

Mesh* ScratchEngine::Rendering::Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.UV.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.UV.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.Normal.x = (float)mesh->mNormals[i].x;
		vertex.Normal.y = (float)mesh->mNormals[i].y;
		vertex.Normal.y = (float)mesh->mNormals[i].z;
		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);

	}

	return new Mesh(&vertices[0],vertices.size(), &indices[0], indices.size(), this->device);
}
