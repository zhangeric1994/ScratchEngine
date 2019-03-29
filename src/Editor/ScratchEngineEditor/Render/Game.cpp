﻿#include "Game.h"
#include <string>

using namespace DirectX;

Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true) {
	vertexShader = 0;
	pixelShader = 0;
	vertexShaderShadow = 0;
	pixelShaderShadow = 0;

	mesh = 0;
	meshPlatform = 0;

	entityVector.resize(3);
	for (int countOfVector = 0; countOfVector < entityVector.size(); countOfVector++)
		entityVector[countOfVector] = NULL;

	camera = new Camera();
	physics = new Physics(200);
	simpleMaterial = NULL;

	directionalLight.AmbientColor = XMFLOAT3(1.0f,  1.0f, 1.0f);
	directionalLight.DiffuseColor = XMFLOAT3(1.0f,  1.0f, 1.0f);
	directionalLight.Direction	  = XMFLOAT3(0.0f,  -1.0f, 0.0f);
	directionalLight.CameraX	  = camera->getPosition().x;
	directionalLight.CameraY	  = camera->getPosition().y;
	directionalLight.CameraZ	  = camera->getPosition().z;

	directionalLight1.AmbientColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight1.DiffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight1.Direction = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	directionalLight1.CameraX = camera->getPosition().x;
	directionalLight1.CameraY = camera->getPosition().y;
	directionalLight1.CameraZ = camera->getPosition().z;

	pointLight.AmbientColor = XMFLOAT3(1.0f,  1.0f,   1.0f);
	pointLight.DiffuseColor = XMFLOAT3(0.4f,  0.4f,   0.4f);
	pointLight.Position		= XMFLOAT3(0.0f, 10.0f,   0.0f);
	pointLight.CameraX = camera->getPosition().x;
	pointLight.CameraY = camera->getPosition().y;
	pointLight.CameraZ = camera->getPosition().z;

	spotLight.AmbientColor = XMFLOAT4(0.2f,  0.2f, 0.2f, 1.0f);
	spotLight.DiffuseColor = XMFLOAT4(1.0f,  1.0f, 1.0f, 1.0f);
	spotLight.Position	   = XMFLOAT3(0.0f,  2.0f, 0.0f);
	spotLight.Direction	   = XMFLOAT3(0.0f, -1.0f, 0.0f);
	spotLight.Cone		   = 0.0f;
	spotLight.Range		   = 1000.0f;

	texture = NULL;
	normalMap = NULL;
	sampler = NULL;

	samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//shadow map

	//shdaow map desc
	shadowMapDesc = {};
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = 1024;
	shadowMapDesc.Width = 1024;
	
	depthStencilView = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	comparisonSamplerDesc = {};
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	drawingRenderStateDesc = {};
	drawingRenderStateDesc.CullMode = D3D11_CULL_BACK;
	drawingRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	drawingRenderStateDesc.DepthClipEnable = true;

	shadowRenderStateDesc = {};
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	shadowViewport = {};
	shadowViewport.Height = 1024;
	shadowViewport.Width = 1024;
	shadowViewport.MinDepth = 0.f;
	shadowViewport.MaxDepth = 1.f;
}

Game::~Game() {
	for (auto& m : entityVector) delete m;

	delete vertexShader;
	delete pixelShader;
	if (mesh) delete mesh;
	delete camera;
	if (simpleMaterial) delete simpleMaterial;
	if (sampler) sampler->Release();
	if (texture) texture->Release();
	if (normalMap) normalMap->Release();
	if (meshPlatform) delete meshPlatform;
	if (shadowMap) shadowMap->Release();
	if (shadowResourceView) shadowResourceView->Release();
	if (shadowRenderState) shadowRenderState->Release();
}

void Game::Init() {
	LoadShaders();
	CreateMatrces();
	CreateBasicGeometry();
}

void Game::LoadShaders() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	std::string spath = std::string(buffer).substr(0, pos).c_str();
	std::wstring wpath = std::wstring(spath.begin(), spath.end());
	std::wstring wVertex = wpath + std::wstring(L"/VertexShader.cso");
	std::wstring wPixel = wpath + std::wstring(L"/PixelShader.cso");
	const wchar_t* vertex = wVertex.c_str();
	const wchar_t* pixel = wPixel.c_str();

	std::wstring sVertex = wpath + std::wstring(L"/VertexShaderShadow.cso");
	std::wstring sPixel = wpath + std::wstring(L"/PixelShaderShadow.cso");

	const wchar_t* vertexShadow = sVertex.c_str();
	const wchar_t* pixelShadow = sPixel.c_str();

	vertexShader = new SimpleVertexShader(device, context);

	vertexShader->LoadShaderFile(vertex);

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(pixel);

	vertexShaderShadow = new SimpleVertexShader(device, context);
	vertexShaderShadow->LoadShaderFile(vertexShadow);

	pixelShaderShadow = new SimplePixelShader(device, context);
	pixelShaderShadow->LoadShaderFile(pixelShadow);
}

void Game::CreateMatrces() {
	//shdaow map light wvp
	XMMATRIX lightPerspectiveMatrix = XMMatrixPerspectiveFovRH(
		XM_PIDIV2,
		1.0f,
		12.f,
		50.f
	);

	XMStoreFloat4x4(
		&lightProjectionMatrix,
		XMMatrixTranspose(lightPerspectiveMatrix)
	);

	XMVECTOR eye = { 0.0f, 20.0f, 0.0f, 0.0f };
	XMVECTOR at = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR upL = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(
		&lightViewMatrix,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, at, upL))
	);

	XMStoreFloat4(&lightPosition, eye);
	//

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); 
	
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	XMMATRIX projection = camera->UpdateProjection((float)width / height);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(projection)); 
}

void Game::CreateBasicGeometry() {
	//shadow map
	device->CreateTexture2D(
		&shadowMapDesc,
		nullptr,
		&shadowMap
	);

	device->CreateDepthStencilView(
		shadowMap,
		&depthStencilViewDesc,
		&shadowdepthStencilView
	);

	device->CreateShaderResourceView(
		shadowMap,
		&shaderResourceViewDesc,
		&shadowResourceView
	);

	device->CreateRasterizerState(
		&shadowRenderStateDesc,
		&shadowRenderState
	);


	//

	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/WhiteMarble_COLOR.jpg", 0, &texture);
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/WhiteMarble_NRM.jpg", 0, &normalMap);

	/*CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rock.jpg", 0, &texture);
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rockNormals.jpg", 0, &normalMap);*/

	device->CreateSamplerState(&samplerDesc, &sampler);
	simpleMaterial = new Material(vertexShader, pixelShader, texture, normalMap, sampler);

	char* filename = (char*)"../Assets/Models/sphere.obj";
	mesh = new Mesh(device, filename);
	filename = (char*)"../Assets/Models/cube.obj";
	meshPlatform = new Mesh(device, filename);

	Entity* temp = new Entity(mesh, simpleMaterial);
	Entity* temp1 = new Entity(meshPlatform, simpleMaterial);
	Entity* temp2 = new Entity(meshPlatform, simpleMaterial);

	entityVector[0] = temp;
	entityVector[1] = temp1;
	entityVector[2] = temp2;

	temp->SetTranslation(-2, 0, 0);
	//temp->SetScale(1.5f, 1.5f, 1.5f);
	temp1->SetTranslation(2, 0, 0);
	temp2->SetTranslation(0, -2, 0);
	//temp2->SetRotation(90, 0, 0);
	temp2->SetScale(10, 1, 10);

	Collider* collider = physics->addCollider(temp, 0.5f, 1.0f, false, false);
	Collider* collider1 = physics->addCollider(temp1, 0.5f, 1.0f, false, false);
	collider->ApplyForce({ 0.9f, 0.0f, 0.0f });
	collider1->ApplyForce({ -0.9f, 0.0f, 0.0f });
}

void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	XMMATRIX projection = camera->UpdateProjection((float)width / height);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(projection)); 
}

void Game::Update(float deltaTime, float totalTime) {
	if (GetAsyncKeyState(VK_ESCAPE)) Quit();

	XMMATRIX view = camera->Update();
	
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Game::Draw(float deltaTime, float totalTime) {
	//backgroud color
	//const float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//-set backgroud color
	//-clear depth buffer
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	physics->CollisionsDetection(0, physics->NumCoolidersHandled, deltaTime);
	//-------------------------------------
	
	//first pass shadow map
	context->OMSetRenderTargets(
		0,
		nullptr,
		shadowdepthStencilView
	);


	context->RSSetState(shadowRenderState);
	context->RSSetViewports(1, &shadowViewport);





	//second pass render scene based on shadow map
	//for (auto& m : entityVector) {
	//	m->SetWorldMatrix();
	//	m->PrepareMatrix(viewMatrix, projectionMatrix);
	//	//m->SetPointLight(pointLight, "pointLight");
	//	m->SetLight(directionalLight, "light");
	//	//m->SetLight(directionalLight1, "light1");
	//	//m->SetSpotLight(spotLight, "spotLight");
	//	m->SetTexture("diffuseTexture", "basicSampler");
	//	m->SetNormalMap("normalMap");
	//	m->CopyAllBufferData();
	//	m->SetShader();

	//	//set vertex buffer and index buffer inside entity class
	//	m->Draw(context);
	//}

	//-------------------------------------

	//End of rendering one frame
	swapChain->Present(0, 0);

	//context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y) {
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
void Game::OnMouseUp(WPARAM buttonState, int x, int y) {
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
void Game::OnMouseMove(WPARAM buttonState, int x, int y) {
	// Add any custom code here...
	if (buttonState & 0x0001) {
		camera->SetRotationX((y - prevMousePos.y) * 0.005f);
		camera->SetRotationY((x - prevMousePos.x) * 0.005f);
	}
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y) {
	// Add any custom code here...
}
#pragma endregion

