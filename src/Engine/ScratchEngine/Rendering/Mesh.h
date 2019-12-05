#pragma once

#include <d3d11_2.h>
#include <fstream>
#include <vector>
#include <DirectXMath.h>

#include "Vertex.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

#include "../Common/Typedefs.h"

using namespace DirectX;


namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) Mesh
		{
			friend class RenderingEngine;


		private:
			ID3D11Buffer* vertexBuffer;
			ID3D11Buffer* indexBuffer;
			i32 indicesNum;
			f32 maxSize;

			void ComputeTangent(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber);

		public:
			Mesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device2* device);
			Mesh(ID3D11Device2* device, char* filename);
			Mesh(const Mesh & mesh);
			~Mesh();

			void* operator new(size_t size);
			void operator delete(void* p);

			ID3D11Buffer* GetVertexBuffer();
			ID3D11Buffer* GetIndexBuffer();
			int GetIndexCount();
			f32 GetMaxSize();

			bool loadFile(ID3D11Device2* device, const std::string& filename);

			void CreateMesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device2* device);
		};
	}
}
