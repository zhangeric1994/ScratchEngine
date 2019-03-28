#pragma once

#include <DirectXMath.h>
#include <vector>
#include <map>
#include "DXCore.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "Vertex.h"
#include "../Physics/Physics.h"
#include "../InputManager.h"
using namespace Colliders;

class Game
	: public DXCore
{
public:
	Game(HINSTANCE hInstance, char* name);
	~Game();

	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	//void OnMouseDown(WPARAM buttonState, int x, int y);
	//void OnMouseUp(WPARAM buttonState, int x, int y);
	//void OnMouseMove(WPARAM buttonState, int x, int y);
	//void OnMouseWheel(float wheelDelta, int x, int y);

	// keyboard input
	bool IsKeyDown(char);
	bool IsKeyDown(PBYTE);
	void SetKeyState(char, bool);
	void Clear();

	// mouse input
	void OnMouseDown(WPARAM, int, int);
	void OnMouseUp(WPARAM, int, int);
	void OnMouseDrag(WPARAM);

private:

	void LoadShaders();
	void CreateMatrces();
	void CreateBasicGeometry();

	POINT prevMousePos;

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	Mesh* mesh;

	Mesh* meshPlatform;


	CollisionManager* physics;

	std::vector<Entity*> entityVector;

	Camera* camera;

	Material* simpleMaterial;

	DirectionalLight directionalLight;
	DirectionalLight directionalLight1;

	PointLight pointLight;

	SpotLight spotLight;

	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* normalMap;

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDesc;

	HWND hWnd;
	char* memOfKey;
	std::map<PBYTE, bool> mapOfKeyCodes;
	POINT mPos;
};