#pragma once
#include <d3d11.h>
#include <fstream> 
#include <vector>
#include <DirectXMath.h>
#include "Mesh.h"

#include "../Common/Typedefs.h"
#include "Vertex.h"

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
			std::vector<Model*> childModels;
			std::vector<Mesh*> meshs;
			Model(ID3D11Device* device);
			Model(ID3D11Device* device, const std::string & filePath);
			~Model();

			bool LoadModel(const std::string & filePth);
			void ProcessNode(aiNode * node, const aiScene * scene);
			Mesh* ProcessMesh(aiMesh * mesh, const aiScene * scene);
		};
	}
}
