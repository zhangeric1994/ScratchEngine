#pragma once
#include <d3d11.h>
#include <fstream> 
#include <vector>
#include <DirectXMath.h>
#include "Rendering/Mesh.h"

#include "Common/Typedefs.h"
#include "Rendering/Vertex.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Model
		{
			friend class RenderingEngine;

		private:
			std::vector<Mesh*> meshes;
			ID3D11Device * device;
		public:
			Model(const std::string & filePath,ID3D11Device* device);
			~Model();

			void* operator new(size_t size);
			void operator delete(void* p);

			bool LoadModel(const std::string & filePth);
			void ProcessNode(aiNode * node, const aiScene * scene);
			Mesh* ProcessMesh(aiMesh * mesh, const aiScene * scene);
			void CreateModel(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device* device);
		};
	}
}
