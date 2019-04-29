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
	this->anim = nullptr;
	this->LoadModel(filePath);
}

ScratchEngine::Rendering::Model::~Model()
{
}

bool ScratchEngine::Rendering::Model::LoadAnimation(const string & filePath)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_ConvertToLeftHanded
	);
	if (pScene == NULL)
		return false;

	return anim->LoadAnimations(pScene);
}

bool ScratchEngine::Rendering::Model::LoadModel(const std::string & filePath)
{
	vertices.clear();
	indices.clear();

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

	mesh = new Mesh(&vertices[0], vertices.size(), & indices[0], indices.size(), device);
	return true;
}

void ScratchEngine::Rendering::Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if(anim&&anim->hasSkeleton)ExtractBoneWeightsFromMesh(mesh);
		this->ProcessMesh(mesh, scene);
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->ProcessNode(node->mChildren[i], scene);
		//childModels.push_back(this);
	}

	/*for (UINT i = 0; i < node->mNumChildren; i++) {
		Model* child = new Model(device, this->vertToBoneWeight);
		child->ProcessNode(node->mChildren[i], scene);
		childModels.push_back(child);
	}*/
}

void ScratchEngine::Rendering::Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	int verterxStart = vertices.size();

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
			vertex.UV.y = 1.0f - (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.Normal.x = (float)mesh->mNormals[i].x;
		vertex.Normal.y = (float)mesh->mNormals[i].y;
		vertex.Normal.z = (float)mesh->mNormals[i].z;

		if (anim && anim->hasSkeleton)
		{
			auto vertexWeight = vertToBoneWeight[(u32)i];

			int numBones = vertexWeight->size();
			f32 w = 0;

			for (UINT j = 0; j < NUM_BONES_PER_VEREX; j++)
			{
				if (j < numBones)
				{
					vertex.IDs[j] = vertexWeight->at(j)->mVertexId;
					//if (vertexWeight->at(j)->mWeight + w > 1) {
					//	/*printf("size : %f \n", numBones);
					//	printf("previous weight : %f \n",w);
					//	printf("BONE WEIGHT ERROR!!  weight : %f \n", vertexWeight->at(j)->mWeight);*/
					//	vertex.Weights[j] = 1;
					//	
					//}else {
					//	vertex.Weights[j] = vertexWeight->at(j)->mWeight;
					//	w += vertex.Weights[j];
					//}
					vertex.Weights[j] = vertexWeight->at(j)->mWeight;
					w += vertex.Weights[j];
				}
				else
				{
					vertex.IDs[j] = 0;
					vertex.Weights[j] = 0;
				}
			}

			for (UINT j = 0; j < NUM_BONES_PER_VEREX; j++) {
				vertex.Weights[j] /= w;
			}
			//for (UINT j = 0; j < NUM_BONES_PER_VEREX; j++)
			//	vertex.Weights[j] /= w;
		}
		
		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j] + verterxStart);
	}

}

void ScratchEngine::Rendering::Model::ExtractBoneWeightsFromMesh(aiMesh* mesh)
{
	for (UINT i = 0; i < mesh->mNumBones;i++)
	{
		aiBone* bone = mesh->mBones[i];
		int boneIndex = anim->GetBoneIndex(bone->mName.C_Str());

		for (UINT j = 0; j < bone->mNumWeights; j++) {
			aiVertexWeight* weight = &(bone->mWeights[j]);

			std::map<UINT, std::vector<aiVertexWeight*>*>::iterator it = vertToBoneWeight.find(weight->mVertexId);
			
			if (it == vertToBoneWeight.end())
			{
				vector<aiVertexWeight*>* v = new std::vector<aiVertexWeight*>();
				v->push_back(new aiVertexWeight(boneIndex, weight->mWeight));

				vertToBoneWeight[(UINT)weight->mVertexId] = v;
			}
			else
				it->second->push_back(new aiVertexWeight(boneIndex, weight->mWeight));
		}
	}
}
