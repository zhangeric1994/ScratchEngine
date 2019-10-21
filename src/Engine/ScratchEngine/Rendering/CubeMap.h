#pragma once
#include <d3d11_2.h>
#include <DDSTextureLoader.h>

#include "Mesh.h"
#include "SimpleShader.h"

#include "../Common/Typedefs.h"


namespace ScratchEngine
{
	namespace Rendering
	{
		struct __declspec(dllexport) CubeMap
		{
		public:
			CubeMap();
			~CubeMap();

			bool setUp(ID3D11Device2* device);
			void setSampler(ID3D11SamplerState* _sampler);
			void setMesh(Mesh* _cubeMesh);
			void setPS(ID3D11Device2* device, ID3D11DeviceContext2* context, LPCWSTR path);
			void setVS(ID3D11Device2* device, ID3D11DeviceContext2* context, LPCWSTR path);
			void setSRV(ID3D11Device2* device, ID3D11DeviceContext2* context, const wchar_t* path);

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