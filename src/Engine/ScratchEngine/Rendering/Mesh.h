#pragma once

#include <d3d11.h>
#include <fstream> 
#include <vector>
#include <DirectXMath.h>

#include "../Common/Typedefs.h"

#include "Vertex.h"

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


		public:
			Mesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device* device);
			Mesh(ID3D11Device* device, char* filename);
			Mesh(const Mesh & mesh);
			~Mesh();

			void* operator new(size_t size);
			void operator delete(void* p);

			ID3D11Buffer* GetVertexBuffer();
			ID3D11Buffer* GetIndexBuffer();
			int GetIndexCount();

			void CreateMesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device* device);
		};
	}
}
