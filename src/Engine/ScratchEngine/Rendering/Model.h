#pragma once
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <fstream>
#include <map>
#include <vector>

#include "Mesh.h"
#include "Vertex.h"

#include "../Animation/Animator.h"
#include "../Common/Typedefs.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace DirectX;
using namespace ScratchEngine::Animation;
using namespace ScratchEngine::Rendering;
using namespace std;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Model
		{
			friend class RenderingEngine;

		private:
			ID3D11Device2* device;

		public:
			string name;
			vector<Vertex> vertices;
			vector<UINT> indices;
			const aiScene* rawData;

			Animator* anim;
			
			map<UINT, vector<aiVertexWeight*>*> vertToBoneWeight;
			//std::vector<Model*> childModels;
			Mesh* mesh;
			Model(ID3D11Device2* device, map<UINT, vector<aiVertexWeight*>*> vertToBone);
			Model(ID3D11Device2* device, const string& filePath);
			~Model();
			
			bool LoadAnimation(const string& filePath);
			bool LoadModel(const string& filePth);
			void ProcessNode(aiNode* node, const aiScene* scene);
			void ProcessMesh(aiMesh* mesh, const aiScene* scene);
		    void ExtractBoneWeightsFromMesh(aiMesh* mesh);
		};
	}
}
