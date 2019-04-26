#include <string>

#include "Game.h"

#include "../Core/Global.h"
#include "../Core/Scene.h"
#include "../Physics/PhysicsEngine.h"
#include "../Rendering/RenderingEngine.h"
#include "../Rendering/Mesh.h"

using namespace DirectX;
using namespace ScratchEngine;
using namespace ScratchEngine::Physics;
using namespace ScratchEngine::Rendering;


Material* ScratchEngine::Game::greenMaterial = nullptr;
Material* ScratchEngine::Game::redMaterial = nullptr;

ScratchEngine::Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true), frameBarrier(2)
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	vsZPrepass = nullptr;

	greenMaterial = nullptr;
	redMaterial = nullptr;

	zPrepassDepthStencilState = nullptr;
	
	sphereMesh = nullptr;
	cubeMesh = nullptr;

	sampler = 0;
	texture = 0;
	normalMap = 0;
	metalnessMap = 0;
	roughnessMap = 0;

	shadow = new ShadowMap();
	cubeMap = new CubeMap();

	shadowMapSize = 1024;

	shadowViewport = {};
	shadowViewport.Height = shadowMapSize;
	shadowViewport.Width = shadowMapSize;
	shadowViewport.MinDepth = 0.f;
	shadowViewport.MaxDepth = 1.f;
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;

	samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Global::SetScreenRatio(1280.0f / 720.0f);

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

ScratchEngine::Game::~Game() {
	if (vertexShader)
		delete vertexShader;

	if (pixelShader)
		delete pixelShader;
	
	if (vsZPrepass)
		delete vsZPrepass;

	if (pbrMaterial)
		delete pbrMaterial;

	if (greenMaterial)
		delete greenMaterial;

	if (redMaterial)
		delete redMaterial;

	if (zPrepassDepthStencilState)
		zPrepassDepthStencilState->Release();

	delete Scene::GetCurrentScene();

	if (cubeMesh)
		delete cubeMesh;
	
	if (sphereMesh)
		delete sphereMesh;

	if (texture)
		texture->Release();

	if (sampler)
		sampler->Release();

	if (normalMap)
		normalMap->Release();

	if (shadow)
		delete shadow;

	if (cubeMap)
		delete cubeMap;

	if (roughnessMap)
		roughnessMap->Release();

	if (metalnessMap)
		metalnessMap->Release();

	if (pixelShaderPBR)
		delete pixelShaderPBR;

	if (shadowShader)
		delete shadowShader;


	RenderingEngine::Terminate();
}

void ScratchEngine::Game::Init()
{
	PhysicsEngine::Initialize();
	RenderingEngine::Initialize(device, context);

	LoadShaders();
	CreateMatrces();
	CreateBasicGeometry();
	CreateAllMaps();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ScratchEngine::Game::LoadShaders()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	std::string spath = std::string(buffer).substr(0, pos).c_str();
	std::wstring wpath = std::wstring(spath.begin(), spath.end());

	//vsZPrepass = new SimpleVertexShader(device, context);
	//vsZPrepass->LoadShaderFile((wpath + std::wstring(L"/vs_zprepass.cso")).c_str());

	shadowShader = new SimpleVertexShader(device, context);
	shadowShader->LoadShaderFile((wpath + std::wstring(L"/shadowVS.cso")).c_str());
	
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile((wpath + std::wstring(L"/VertexShader.cso")).c_str());

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile((wpath + std::wstring(L"/PixelShader.cso")).c_str());

	pixelShaderPBR = new SimplePixelShader(device, context);
	pixelShaderPBR->LoadShaderFile((wpath + std::wstring(L"/PixelShaderPBR.cso")).c_str());

	//cube map shader load
	cubeMap->setPS(device, context, (wpath + std::wstring(L"/cubePS.cso")).c_str());
	cubeMap->setVS(device, context, (wpath + std::wstring(L"/cubeVS.cso")).c_str());
	//end of cube map shader

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, &zPrepassDepthStencilState);
}

void ScratchEngine::Game::CreateAllMaps() {
	//shadow map setup
	shadow->setUp(device);
	//shadow->setShader(vsZPrepass);
	shadow->setShader(shadowShader);
	RenderingEngine* renderingEngine = RenderingEngine::GetSingleton();
	renderingEngine->SetShadowMap(shadow);
	//End of shadow map

	//cube map
	cubeMap->setUp(device);
	cubeMap->setMesh(cubeMesh);
	cubeMap->setSampler(sampler);
	cubeMap->setSRV(device, context, L"../Assets/Textures/CubeMaps/Skybox1.dds");
	//end of cube map
}

void ScratchEngine::Game::CreateMatrces()
{

}

void ScratchEngine::Game::CreateBasicGeometry()
{
	device->CreateSamplerState(&samplerDesc, &sampler);

	HRESULT isok = CreateWICTextureFromFile(device, context, L"../Assets/Textures/PBR/scratched_albedo.png", 0, &texture);
	if (FAILED(isok)) printf("load albedo texture error");

	isok = CreateWICTextureFromFile(device, context, L"../Assets/Textures/PBR/scratched_normals.png", 0, &normalMap);
	if (FAILED(isok)) printf("load normal map error");

	//load roughness map
	isok = CreateWICTextureFromFile(device, context, L"../Assets/Textures/PBR/scratched_roughness.png", 0, &roughnessMap);
	if (FAILED(isok)) printf("load roughness map error");

	//load metalness map
	isok = CreateWICTextureFromFile(device, context, L"../Assets/Textures/PBR/scratched_metal.png", 0, &metalnessMap);
	if (FAILED(isok)) printf("load metalness map failed");


	sphereMesh = new Mesh(device, (char*)"../Assets/Models/sphere.obj");
	cubeMesh = new Mesh(device, (char*)"../Assets/Models/cube.obj");


	pbrMaterial = new Material(vertexShader, pixelShaderPBR, sampler);
	pbrMaterial->setTexture(texture);
	pbrMaterial->setMetalnessMap(metalnessMap);
	pbrMaterial->setNormalMap(normalMap);
	pbrMaterial->setRoughnessMap(roughnessMap);
	//pbrMaterial->setShadowMap(shadow);

	greenMaterial = new Material(vertexShader, pixelShader, nullptr);
	greenMaterial->SetTint(0, 1, 0);

	redMaterial = new Material(vertexShader, pixelShader, nullptr);
	redMaterial->SetTint(1, 0, 0);


	camera = new GameObject();
	camera->AddComponent<Camera>();

	GameObject* directionalLightObject = new GameObject();
	directionalLightObject->SetLocalRotation(90, 0, 0);
	directionalLight = directionalLightObject->AddComponent<DirectionalLight>();

	go1 = new GameObject();
	go1->SetName("1");
	go1->SetPosition(0, 0, 15);
	go1->SetLocalRotation(45, 0, 90);
	go1->SetLocalScale(1, 2, 1);
	go1->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go1->AddComponent<BoxCollider>();


	go2 = new GameObject();
	go2->SetName("2");
	go2->SetParent(go1);
	go2->SetLocalPosition(0, 4, 0);
	go2->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go2->AddComponent<BoxCollider>();


	GameObject* go3 = new GameObject();
	go3->SetName("3");
	go3->SetParent(go2);
	go3->SetLocalPosition(0, 2, 0);
	go3->AddComponent<Renderer>(pbrMaterial, sphereMesh);

	go4 = new GameObject();
	go4->SetName("4");
	go4->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go4->AddComponent<BoxCollider>();

	go5 = new GameObject();
	go5->SetName("5");
	go5->AddComponent<Renderer>(greenMaterial, sphereMesh);
	go5->AddComponent<SphereCollider>();

	go6 = new GameObject();
	go6->SetName("6");
	go6->SetPosition(0, 0, -15);
	go6->SetLocalRotation(45, 0, 90);
	go6->SetLocalScale(1, 2, 1);
	go6->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go6->AddComponent<BoxCollider>();

	go7 = new GameObject();
	go7->SetName("7");
	go7->SetParent(go6);
	go7->SetLocalPosition(0, 4, 0);
	go7->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go7->AddComponent<BoxCollider>();

	GameObject* go8 = new GameObject();
	go8->SetName("8");
	go8->SetParent(go7);
	go8->SetLocalPosition(0, 2, 0);
	go8->AddComponent<Renderer>(pbrMaterial, sphereMesh);

	go9 = new GameObject();
	go9->SetName("9");
	go9->AddComponent<Renderer>(greenMaterial, cubeMesh);
	go9->AddComponent<BoxCollider>();

	go10 = new GameObject();
	go10->SetName("10");
	go10->AddComponent<Renderer>(greenMaterial, sphereMesh);
	go10->AddComponent<SphereCollider>();

	GameObject* go11 = new GameObject();
	go11->SetLocalPosition(0, -5, 0);
	go11->SetLocalScale(10, 1, 10);
	go11->AddComponent<Renderer>(pbrMaterial, cubeMesh);

	GameObject* go12 = new GameObject();
	go12->SetParent(go11);
	go12->SetLocalPosition(0, 10, 0);
	go12->AddComponent<Renderer>(pbrMaterial, sphereMesh);
}

void ScratchEngine::Game::OnResize()
{
	DXCore::OnResize();

	Global::SetScreenRatio((float)width / height);
}

void ScratchEngine::Game::Update()
{
	while (isRunning)
	{
		UpdateTimer();

		if (titleBarStats)
			UpdateTitleBarStats();

		frameBarrier.Wait();

		if (GetAsyncKeyState(VK_ESCAPE))
			Quit();
			
		if (GetAsyncKeyState('W') & 0x8000)
			camera->Translate(0.0f, 0.0f, 10 * deltaTime);

		if (GetAsyncKeyState('A') & 0x8000)
			camera->Translate(-10 * deltaTime, 0.0f, 0.0f);

		if (GetAsyncKeyState('S') & 0x8000)
			camera->Translate(0.0f, 0.0f, -10 * deltaTime);

		if (GetAsyncKeyState('D') & 0x8000)
			camera->Translate(10 * deltaTime, 0.0f, 0.0f);

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			camera->Translate(0.0f, 10 * deltaTime, 0.0f);

		if (GetAsyncKeyState('X') & 0x8000)
			camera->Translate(0.0f, -10 * deltaTime, 0.0f);

		go1->Rotate(0, 0, 20 * deltaTime);
		go2->Rotate(0, 0, -50 * deltaTime);
		go4->SetLocalPosition(0, 5 * sin(totalTime), 15);
		go5->SetLocalPosition(5 * cos(totalTime), 0, 15);

		go6->Rotate(0, 0, 20 * deltaTime);
		go7->Rotate(0, 0, -50 * deltaTime);
		go9->SetLocalPosition(0, 5 * sin(totalTime), -15);
		go10->SetLocalPosition(5 * cos(totalTime), 0, -15);

		PhysicsEngine* physicsEngine = PhysicsEngine::GetSingleton();

		physicsEngine->UpdateBoundingVolumes();
		physicsEngine->SolveCollisions();


		frameBarrier.Wait();
	}

	allThreadBarrier.Wait();
}

void ScratchEngine::Game::Draw()
{
	RenderingEngine* renderingEngine = RenderingEngine::GetSingleton();

	while (isRunning)
	{
		renderingEngine->UpdateRenderables();
		renderingEngine->UpdateViewers();
		renderingEngine->UpdateLightSources();
		renderingEngine->SortRenderables();

		frameBarrier.Wait();

		const float color[4] = { 0.0f, 0.0f, 0.0f, 0 };

		context->ClearRenderTargetView(backBufferRTV, color);
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		bool hasShadowMap = false;

		shadowViewport.Height = shadowMapSize;
		shadowViewport.Width = shadowMapSize;
		context->RSSetViewports(1, &shadowViewport);


		hasShadowMap = renderingEngine->RenderShadowMap();

		context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
		
		shadowViewport.Width = (float)width;
		shadowViewport.Height = (float)height;
		
		context->RSSetViewports(1, &shadowViewport);
		context->RSSetState(0);

		renderingEngine->PerformZPrepass();
		renderingEngine->DrawForward();


		renderingEngine->RenderCubeMap(cubeMap);

		//turn off all resources bound to shader
		ID3D11ShaderResourceView* noSRV[16] = {};
		context->PSSetShaderResources(0, 16, noSRV);

		swapChain->Present(0, 0);


		frameBarrier.Wait();
	}

	allThreadBarrier.Wait();
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (buttonState & 0x0001)
	{
		//camera->SetRotationX((y - prevMousePos.y) * 0.001f);
		//camera->SetRotationY((x - prevMousePos.x) * 0.001f);
	}

	if (buttonState & 0x0002)
		camera->Rotate((y - prevMousePos.y) * 5 / 31.41592653579f , (x - prevMousePos.x) * 5 / 31.41592653579f, 0.0f);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion

