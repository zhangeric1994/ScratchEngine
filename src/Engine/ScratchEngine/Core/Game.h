#pragma once

#include <DirectXMath.h>
#include <vector>

#include "../Core/GameObject.h"
#include "../Rendering/Camera.h"
#include "../Rendering/Light.h"
#include "../Rendering/Material.h"
#include "../Rendering/Mesh.h"
#include "../Rendering/Model.h"
#include "../Rendering/SimpleShader.h"
#include "../Rendering/Vertex.h"
#include "../Multithreading/Barrier.h"
#include "../Animation/AnimationEngine.h"

#include "DXCore.h"

using namespace ScratchEngine::Rendering;
using namespace ScratchEngine::Multithreading;
using namespace ScratchEngine::Animation;

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
		SimpleVertexShader* vsSkeleton;
		SimplePixelShader* pixelShader;

		ID3D11DepthStencilState* zPrepassDepthStencilState;

		Mesh* sphereMesh;
		Mesh* cubeMesh;

		Model* model;

		DirectionalLight* directionalLight;
		GameObject* camera;
		GameObject* go1;
		GameObject* go2;
		GameObject* go4;

		Barrier frameBarrier;

		
	public:
		static Material* greenMaterial;
		static Material* redMaterial;
	};
}