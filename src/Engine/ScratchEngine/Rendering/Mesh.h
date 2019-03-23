#pragma once
#include <d3d11.h>
#include "Vertex.h"
#include <fstream> 
#include <vector>
#include <DirectXMath.h>
#include "../Utility/Typedefs.h"

using namespace DirectX;

namespace ScratchEngine
{
	namespace Rendering
	{
		struct Mesh {
		public:
			Mesh(Vertex* vertices,
				int verticesNumber,
				unsigned int* indices,
				int indicesNumber,
				ID3D11Device* device);

			Mesh(ID3D11Device* device,
				char* filename);

			~Mesh();

			ID3D11Buffer* GetVertexBuffer();
			ID3D11Buffer* GetIndexBuffer();
			int GetIndexCount();

			void CreateMesh(Vertex* vertices,
				int verticesNumber,
				unsigned int* indices,
				int indicesNumber,
				ID3D11Device* device);

		private:
			ID3D11Buffer* vertexBuffer;
			ID3D11Buffer* indexBuffer;
			i32 indicesNum;
		};
	}
}
