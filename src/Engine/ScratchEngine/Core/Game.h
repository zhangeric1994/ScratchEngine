#pragma once

#include <DDSTextureLoader.h>
#include <DirectXMath.h>
#include <vector>

#include "../Multithreading/Barrier.h"
#include "../Rendering/CubeMap.h"
#include "../Rendering/Material.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/ShadowMap.h"
#include "../Rendering/SimpleShader.h"
#include "../Rendering/Vertex.h"

#include "Camera.h"
#include "DXCore.h"
#include "GameObject.h"
#include "Light.h"

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
		void CreateAllMaps();

		POINT prevMousePos;

		f32 camX;
		f32 camY;

		SimpleVertexShader* vsZPrepass;
		SimpleVertexShader* vertexShader;
		SimplePixelShader* pixelShader;
		SimplePixelShader* pixelShaderPBR;

		ID3D11DepthStencilState* zPrepassDepthStencilState;

		Material* pbrMaterial;

		Mesh* sphereMesh;
		Mesh* cubeMesh;

		ID3D11SamplerState* sampler;
		D3D11_SAMPLER_DESC samplerDesc;

		ID3D11ShaderResourceView* texture;
		ID3D11ShaderResourceView* normalMap;

		DirectionalLight* directionalLight;
		GameObject* camera;
		GameObject* go1;
		GameObject* go2;
		GameObject* go4;
		GameObject* go5;
		GameObject* go6;
		GameObject* go7;
		GameObject* go9;
		GameObject* go10;

		Barrier frameBarrier;


		//final shadow map
		ShadowMap* shadow;
		D3D11_VIEWPORT shadowViewport;
		float shadowMapSize;
		SimpleVertexShader* shadowShader;

		//cube map
		CubeMap* cubeMap;

		//PBR textures
		ID3D11ShaderResourceView* roughnessMap;
		ID3D11ShaderResourceView* metalnessMap;

		
	public:
		static Material* greenMaterial;
		static Material* redMaterial;
	};
}