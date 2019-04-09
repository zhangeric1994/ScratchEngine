#include <string>

#include "Game.h"

#include "../Core/Global.h"
#include "../Core/Scene.h"

#include "../Rendering/RenderingEngine.h"
#include "../Rendering/Mesh.h"

using namespace DirectX;
using namespace ScratchEngine;
using namespace ScratchEngine::Rendering;

ScratchEngine::Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true), frameBarrier(2)
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	vsZPrepass = nullptr;

	simpleMaterial = nullptr;

	zPrepassDepthStencilState = nullptr;
	
	mesh = nullptr;
	mesh1 = nullptr;

	sampler = 0;
	texture = 0;
	normalMap = 0;

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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Global::SetScreenRatio(1280.0f / 720.0f);

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

ScratchEngine::Game::~Game()
{
	if (vertexShader)
		delete vertexShader;

	if (pixelShader)
		delete pixelShader;
	
	if (vsZPrepass)
		delete vsZPrepass;

	if (simpleMaterial)
		delete simpleMaterial;

	if (zPrepassDepthStencilState)
		zPrepassDepthStencilState->Release();

	delete Scene::GetCurrentScene();

	if (mesh)
		delete mesh;
	
	if (mesh1)
		delete mesh1;

	if (texture) texture->Release();

	if (sampler) sampler->Release();

	if (normalMap) normalMap->Release();

	if (shadow) delete shadow;

	if (cubeMap) delete cubeMap;

	RenderingEngine::Stop();
}

void ScratchEngine::Game::Init()
{
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
	std::wstring wVertex = wpath + std::wstring(L"/VertexShader.cso");
	std::wstring wPixel = wpath + std::wstring(L"/PixelShader.cso");
	const wchar_t* vertex = wVertex.c_str();
	const wchar_t* pixel = wPixel.c_str();

	vsZPrepass = new SimpleVertexShader(device, context);
	vsZPrepass->LoadShaderFile((wpath + std::wstring(L"/vs_zprepass.cso")).c_str());

	shadowShader = new SimpleVertexShader(device, context);
	shadowShader->LoadShaderFile((wpath + std::wstring(L"/shadowVS.cso")).c_str());
	
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(vertex);

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(pixel);

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
	shadow->setShader(shadowShader);
	RenderingEngine* renderingEngine = RenderingEngine::GetSingleton();
	renderingEngine->SetShadowMap(shadow);
	//End of shadow map

	//cube map
	cubeMap->setUp(device);
	cubeMap->setMesh(mesh1);
	cubeMap->setSampler(sampler);
	cubeMap->setSRV(device, context, L"../Assets/Textures/CubeMaps/SunnyCubeMap.dds");
	//end of cube map
}

void ScratchEngine::Game::CreateMatrces()
{

}

void ScratchEngine::Game::CreateBasicGeometry()
{
	//char* filename = (char*)"../Assets/Models/sphere.obj";

	char* filename = (char*)"../Assets/Greninja/XY_Greninja.obj";
	char* cubefile = (char*)"../Assets/Models/cube.obj";

	device->CreateSamplerState(&samplerDesc, &sampler);

	//CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rock.jpg", 0, &texture);
	
	CreateWICTextureFromFile(device, context, L"../Assets/Greninja/Textures/pm0725_00_BodyA1.png", 0, &texture);
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rockNormals.jpg", 0, &normalMap);
	
	mesh = new Mesh(device, filename);
	mesh1 = new Mesh(device, cubefile);

	simpleMaterial = new Material(vertexShader, pixelShader, texture, normalMap, sampler);

	camera = new GameObject();
	camera->AddComponent<Camera>();

	GameObject* directionalLightObject = new GameObject();
	//directionalLightObject->SetRotation(90, 0, 0);
	directionalLight = directionalLightObject->AddComponent<DirectionalLight>();

	go1 = new GameObject();
	go1->SetPosition(0, -2, 10);
	//go1->SetLocalRotation(0, 0, 0);
	//go1->SetLocalScale(.1, .1, .1);
	go1->AddComponent<Renderer>(simpleMaterial, mesh1);

	go2 = new GameObject();
	go2->SetParent(go1);
	go2->SetLocalPosition(0, 2, 0);
	go2->SetLocalScale(.01, .01, .01);
	go2->AddComponent<Renderer>(simpleMaterial, mesh);

	GameObject* go3 = new GameObject();
	go3->SetParent(go2);
	go3->SetLocalPosition(0, 2, 0);
	go3->AddComponent<Renderer>(simpleMaterial, mesh);

	GameObject* go4 = new GameObject();
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

		if (GetAsyncKeyState(VK_ESCAPE)) Quit();

		if (GetAsyncKeyState('W') & 0x8000)
			camera->Translate(0.0f, 0.0f, deltaTime*5, SELF);

		if (GetAsyncKeyState('A') & 0x8000)
			camera->Translate(-deltaTime*5, 0.0f, 0.0f, SELF);

		if (GetAsyncKeyState('S') & 0x8000)
			camera->Translate(0.0f, 0.0f, -deltaTime*5, SELF);

		if (GetAsyncKeyState('D') & 0x8000)
			camera->Translate(deltaTime*5, 0.0f, 0.0f, SELF);

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			camera->Translate(0.0f, deltaTime*5, 0.0f, SELF);

		if (GetAsyncKeyState('X') & 0x8000)
			camera->Translate(0.0f, -deltaTime*5, 0.0f, SELF);

		//go1->Rotate(0, 0, 20 * deltaTime);
		//go2->Rotate(0, 0, -50 * deltaTime);

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

		shadowViewport.Height = shadowMapSize;
		shadowViewport.Width = shadowMapSize;
		context->RSSetViewports(1, &shadowViewport);

		renderingEngine->RenderShadowMap(context);

		context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
		
		shadowViewport.Width = (float)width;
		shadowViewport.Height = (float)height;
		
		context->RSSetViewports(1, &shadowViewport);
		context->RSSetState(0);


		context->OMSetDepthStencilState(nullptr, 0);
		renderingEngine->PerformZPrepass(vsZPrepass, context); 

		context->OMSetDepthStencilState(zPrepassDepthStencilState, 0);

		renderingEngine->DrawForward(context);

		renderingEngine->RenderCubeMap(context, cubeMap);

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

