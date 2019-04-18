#include "Model.h"
using namespace ScratchEngine::Rendering;
ScratchEngine::Rendering::Model::Model(ID3D11Device * device, std::map<UINT, std::vector<aiVertexWeight*> *> vertToBone)
{
	this->device = device;
	this->vertToBoneWeight = vertToBone;
	anim = nullptr;
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
		aiProcess_GenSmoothNormals|
		aiProcess_ConvertToLeftHanded
	);
	if (pScene == NULL)
		return false;
	rawData = pScene;
	anim = new Animator(rawData);
	name = std::string(rawData->mRootNode->mName.C_Str());
	this->ProcessNode(rawData->mRootNode, rawData);
	return true;
}

void ScratchEngine::Rendering::Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if(anim&&anim->hasSkeleton)ExtractBoneWeightsFromMesh(mesh);
		meshs.push_back(this->ProcessMesh(mesh, scene));
	}

	//for (UINT i = 0; i < node->mNumChildren; i++) {
	//	this->ProcessNode(node->mChildren[i], scene);
	//	//childModels.push_back(this);
	//}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		Model* child = new Model(device, this->vertToBoneWeight);
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

		if (anim && anim->hasSkeleton) {
			for (UINT j = 0; j < vertToBoneWeight[(UINT)i]->size(); j++) {
				if (j > NUM_BONES_PER_VEREX - 1) {
					break;
				}
				else {
					vertex.Bones.IDs[j] = vertToBoneWeight[(UINT)i]->at(j)->mVertexId;
					vertex.Bones.Weights[j] = vertToBoneWeight[(UINT)i]->at(j)->mWeight;
				}
			}
		}

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

void ScratchEngine::Rendering::Model::ExtractBoneWeightsFromMesh(aiMesh* mesh)
{
	for (UINT i = 0; i < mesh->mNumBones;i++) {
		aiBone * bone = mesh->mBones[i];
		int boneIndex = anim->GetBoneIndex(bone->mName.C_Str());
		
		for (UINT j = 0; j < bone->mNumWeights; j++) {
			aiVertexWeight * weight = &(bone->mWeights[j]);
			std::map<UINT, std::vector<aiVertexWeight*>*>::iterator it = vertToBoneWeight.find((UINT)weight->mVertexId);
			if (it != vertToBoneWeight.end()) {
				vertToBoneWeight.at((UINT)weight->mVertexId)->push_back(new aiVertexWeight(boneIndex, weight->mWeight));
			}
			else {
				vertToBoneWeight[(UINT)weight->mVertexId] = new std::vector<aiVertexWeight* >();
				vertToBoneWeight.at((UINT)weight->mVertexId)->push_back(new aiVertexWeight(boneIndex, weight->mWeight));
			}
		}
	}
}
