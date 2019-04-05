#pragma once

#include <DirectXMath.h>
#include <vector>

#include "../Core/GameObject.h"
#include "../Rendering/Camera.h"
#include "../Rendering/Light.h"
#include "../Rendering/Material.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/SimpleShader.h"
#include "../Rendering/Vertex.h"
#include "../Multithreading/Barrier.h"

#include "DXCore.h"

using namespace ScratchEngine::Rendering;
using namespace ScratchEngine::Multithreading;

namespace ScratchEngine
{
	class __declspec(dllexport) Game final : public DXCore
	{
	public:
		Game(HINSTANCE hInstance, char* name);
		~Game();

		void Init();
		void OnResize();
		void Update();
		void Draw();

		void OnMouseDown(WPARAM buttonState, int x, int y);
		void OnMouseUp(WPARAM buttonState, int x, int y);
		void OnMouseMove(WPARAM buttonState, int x, int y);
		void OnMouseWheel(float wheelDelta, int x, int y);

	private:
		void LoadShaders();
		void CreateMatrces();
		void CreateBasicGeometry();

		POINT prevMousePos;

		SimpleVertexShader* vsZPrepass;
		SimpleVertexShader* vertexShader;
		SimplePixelShader* pixelShader;

		ID3D11DepthStencilState* zPrepassDepthStencilState;

		Mesh* mesh;
		Mesh* mesh1;

		Material* simpleMaterial;

		ID3D11SamplerState* sampler;
		D3D11_SAMPLER_DESC samplerDesc;

		ID3D11ShaderResourceView* texture;
		ID3D11ShaderResourceView* normalMap;

		DirectionalLight* directionalLight;
		GameObject* camera;
		GameObject* go1;
		GameObject* go2;

		Barrier frameBarrier;

		//shadow map
		ID3D11Texture2D*     shadowMap;
		D3D11_TEXTURE2D_DESC shadowDesc;

		ID3D11SamplerState* shadowSampler;

		SimpleVertexShader* shadowShader;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		ID3D11DepthStencilView* shadowDepthStencilView;

		ID3D11ShaderResourceView* shadowResourceView;

		D3D11_SAMPLER_DESC comparisonSamplerDesc;

		D3D11_RASTERIZER_DESC drawingRenderStateDesc;

		D3D11_RASTERIZER_DESC shadowRenderStateDesc;

		D3D11_VIEWPORT shadowViewport;

		ID3D11RasterizerState* shadowRasterizerState;
	};
}