#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include <fstream> 
#include <vector>
#include <DirectXMath.h>
#include "../Common/Typedefs.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Rendering
	{
		struct Mesh
		{
			friend class RenderingEngine;


		private:
			ID3D11Buffer* vertexBuffer;
			ID3D11Buffer* indexBuffer;
			i32 indicesNum;


		public:
			Mesh(Vertex* vertices, int verticesNumber, unsigned int* indices, int indicesNumber, ID3D11Device* device);
			Mesh(ID3D11Device* device, char* filename);
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
