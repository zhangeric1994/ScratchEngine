#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <vector>
#include "SimpleShader.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "Vertex.h"
#include "../Physics/Physics.h"

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

	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

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

	Physics* physics;

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

	//shadow map
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ID3D11Texture2D*     shadowMap;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ID3D11DepthStencilView* shadowdepthStencilView;

	ID3D11ShaderResourceView* shadowResourceView;

	D3D11_SAMPLER_DESC comparisonSamplerDesc;

	D3D11_RASTERIZER_DESC drawingRenderStateDesc;

	D3D11_RASTERIZER_DESC shadowRenderStateDesc;

	XMFLOAT4X4 lightViewMatrix;
	XMFLOAT4X4 lightProjectionMatrix;

	XMFLOAT4 lightPosition;
	D3D11_VIEWPORT shadowViewport;

};