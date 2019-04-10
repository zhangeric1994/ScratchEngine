#pragma once
#include <d3d11.h>
#include "SimpleShader.h"
#include "Mesh.h"
#include "../Common/Typedefs.h"
#include <DDSTextureLoader.h>

namespace ScratchEngine {
	namespace Rendering {
		struct __declspec(dllexport) CubeMap {
		public:
			CubeMap();
			~CubeMap();

			bool setUp(ID3D11Device* device);
			void setSampler(ID3D11SamplerState* _sampler);
			void setMesh(Mesh* _cubeMesh);
			void setPS(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR path);
			void setVS(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR path);
			void setSRV(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path);

			ID3D11SamplerState*			getSampler();
			SimpleVertexShader*			getVS();
			SimplePixelShader*			getPS();
			ID3D11Buffer*				getVB();
			ID3D11Buffer*				getIB();
			int							getIndexCount();
			ID3D11RasterizerState*		getRS();
			ID3D11DepthStencilState*	getDSS();
			ID3D11ShaderResourceView*	getSRV();

		private:
			D3D11_RASTERIZER_DESC		cubeRD;
			D3D11_DEPTH_STENCIL_DESC	cubeDSD;

			SimpleVertexShader*			cubeVS;
			SimplePixelShader*			cubePS;

			ID3D11Buffer*				vertexBuffer;
			ID3D11Buffer*				indexBuffer;
			i32							indicesNum;

			ID3D11RasterizerState*		cubeRS;
			ID3D11DepthStencilState*	cubeDSS;
			ID3D11ShaderResourceView*	cubeSRV;

			ID3D11SamplerState*			sampler;
		};
	}
}