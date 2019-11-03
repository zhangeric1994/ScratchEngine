#pragma once

#include <DDSTextureLoader.h>
#include <DirectXMath.h>
#include <vector>
#include <Winuser.h>

#include "../Multithreading/Barrier.h"
#include "../Rendering/Material.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Model.h"
#include "../Rendering/SimpleShader.h"
#include "../Rendering/Vertex.h"
#include "../Rendering/Viewer.h"

#include "Camera.h"
#include "DXCore.h"
#include "GameObject.h"
#include "Light.h"

using namespace ScratchEngine::Rendering;
using namespace ScratchEngine::Multithreading;


namespace ScratchEngine
{
	class Scene;


	class __declspec(dllexport) Game final : public DXCore
	{
	public:
		Game(HINSTANCE hInstance, char* name);
		~Game();

		void Initialize();
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

		//cube map
		TextureCube* sky;

		//PBR textures
		ID3D11ShaderResourceView* roughnessMap;
		ID3D11ShaderResourceView* metalnessMap;

		f32 camX;
		f32 camY;

		SimpleVertexShader* vsZPrepass;
		SimpleVertexShader* vertexShader;
		SimpleVertexShader* vsSkeleton;

		SimplePixelShader* pixelShader;
		SimplePixelShader* psPBR;
		SimplePixelShader* psBlinnPhong;

		ID3D11DepthStencilState* zPrepassDepthStencilState;

		ID3D11ShaderResourceView* GBufferAlbedoSRV;
		ID3D11RenderTargetView* GBufferAlbedoRTV;
		ID3D11ShaderResourceView* GBufferNormalsSRV;
		ID3D11RenderTargetView* GBufferNormalsRTV;
		ID3D11ShaderResourceView* GBufferDepthSRV;
		ID3D11RenderTargetView* GBufferDepthRTV;
		ID3D11ShaderResourceView* GBufferMaterialSRV;
		ID3D11RenderTargetView* GBufferMaterialRTV;
		ID3D11ShaderResourceView* DeferredLightBufferSRV;
		ID3D11RenderTargetView* DeferredLightBufferRTV;

		ID3D11ShaderResourceView* ssaoBufferSRV;
		ID3D11RenderTargetView* ssaoBufferRTV;
		
		Material* pbrMaterial;
		Material* skeletonMaterial; 
		Material* mobMaterial;

		Mesh* sphereMesh;
		Mesh* cubeMesh;

		Model* model;
		Model* mobModel;

		ID3D11SamplerState* sampler;

		ID3D11ShaderResourceView* texture;
		ID3D11ShaderResourceView* normalMap;

		DirectionalLight* directionalLight;
		PointLight* pointLight;
		AmbientLight* ambientLight;

		GameObject* camera;
		GameObject* cameraHolder;
		GameObject* player;
		GameObject* mob;
		GameObject* go1;
		GameObject* go2;
		GameObject* go4;
		GameObject* go5;
		GameObject* go6;
		GameObject* go7;
		GameObject* go9;
		GameObject* go10;

		Renderer* rightHandRenderer;
		SphereCollider* rightHandCollider;

		Renderer* leftHandRenderer;
		SphereCollider* leftHandCollider;

		int lightCount = 0;
		GameObject* lights[128];

		Barrier frameBarrier;

		int renderingMode = 1;

		int comboCounter = 0;
		int combo[5] ={ 14,14,15,16,17 };
		bool attacking = false;

		float lastTriggerTime;

		float lastInputTime;
		float lastAttackTime;
		bool isMouseDownL;
		bool isMouseDownR;


		void __RenderShadows(RenderingEngine* renderingEngine, Scene* scene, Viewer* viewer, D3D11_VIEWPORT* viewport);

		
	public:
		static Material* greenMaterial;
		static Material* redMaterial;
	};
}