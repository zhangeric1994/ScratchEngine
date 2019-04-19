#pragma once
#include <d3d11.h>
#include <fstream> 
#include <vector>
#include <map>
#include <DirectXMath.h>
#include "Mesh.h"

#include "../Common/Typedefs.h"
#include "Vertex.h"
#include "../Animation/Animator.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace DirectX;
using namespace ScratchEngine::Rendering;

namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Model
		{
			friend class RenderingEngine;

		private:
			ID3D11Device * device;
		public:
			std::string name;
			std::vector<Vertex> vertices;
			std::vector<UINT> indices;
			const aiScene * rawData;
			Animator * anim;
			std::map<UINT, std::vector<aiVertexWeight*> *> vertToBoneWeight;
			//std::vector<Model*> childModels;
			Mesh* mesh;
			Model(ID3D11Device* device, std::map<UINT, std::vector<aiVertexWeight*> *> vertToBone);
			Model(ID3D11Device* device, const std::string & filePath);
			~Model();
			
			bool LoadModel(const std::string & filePth);
			void ProcessNode(aiNode * node, const aiScene * scene);
			void ProcessMesh(aiMesh * mesh, const aiScene * scene);
		    void ExtractBoneWeightsFromMesh(aiMesh* mesh);
		};
	}
}
