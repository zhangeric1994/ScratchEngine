#include "Game.h"
#include <string>

using namespace DirectX;

Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true) {
	vertexShader = 0;
	pixelShader = 0;

	mesh = 0;
	mesh1 = 0;
	lastPhysicsCheck = 0.0f;

	entityVector.resize(100);
	for (int countOfVector = 0; countOfVector < entityVector.size(); countOfVector++)
		entityVector[countOfVector] = NULL;

	camera = new Camera();
	physics = new CollisionManager(200);
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


#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
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
	if (mesh1) delete mesh1;
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

	vertexShader = new SimpleVertexShader(device, context);

	vertexShader->LoadShaderFile(vertex);

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(pixel);
}

void Game::CreateMatrces() {
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); 
	
	XMVECTOR pos = XMVectorSet(0, 0, -20, 0);
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
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/WhiteMarble_COLOR.jpg", 0, &texture);
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/WhiteMarble_NRM.jpg", 0, &normalMap);

	/*CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rock.jpg", 0, &texture);
	CreateWICTextureFromFile(device, context, L"../Assets/Textures/WhiteMarble/rockNormals.jpg", 0, &normalMap);*/

	device->CreateSamplerState(&samplerDesc, &sampler);
	simpleMaterial = new Material(vertexShader, pixelShader, texture, normalMap, sampler);

	char* filename = (char*)"../Assets/Models/sphere.obj";
	char* cubefile = (char*)"../Assets/Models/cube.obj";
	mesh = new Mesh(device, filename);
	mesh1 = new Mesh(device, cubefile);
	//Entity* temp = new Entity(mesh, simpleMaterial);
	//Entity* temp1 = new Entity(mesh, simpleMaterial);
	//Entity* temp2 = new Entity(mesh, simpleMaterial);
	//Entity* temp3 = new Entity(mesh, simpleMaterial);
	//Entity* temp4 = new Entity(mesh, simpleMaterial);
	//Entity* temp5 = new Entity(mesh, simpleMaterial);
	Entity* terrain = new Entity(mesh1, simpleMaterial);
	Entity* obstacle = new Entity(mesh1, simpleMaterial);
	//entityVector[0] = temp;
	//entityVector[1] = temp1;
	//entityVector[2] = temp2;
	//entityVector[3] = temp3;
	//entityVector[4] = temp4;
	//entityVector[5] = temp5;
	//entityVector[6] = terrain;
	//temp->SetTranslation(-2, 0, 0);
	//temp1->SetTranslation(2.5, 1.5, 0);
	//temp2->SetTranslation(-2.5, 1, 0);
	//temp3->SetTranslation(0.0, 1, 0);
	//temp4->SetTranslation(-1, -1, 0);
	//temp5->SetTranslation(2, 0, 0);
	//temp1->SetRotation(0.5f, 0.0f, 0.0f);
	obstacle->SetTranslation(0.0f, -9.5f, 0.0f);
	obstacle->SetTranslation(2.0f, -8.0f, 2.0f);
	obstacle->SetScale(3, 3, 3);
	terrain->SetTranslation(0, -10, 0);
	terrain->SetScale(100, 1, 100);
	//Collider* collider = physics->addSphereCollider(temp, 0.5f, 0.5f, true, false);
	//Collider* collider1 = physics->addSphereCollider(temp1, 0.5f, 0.7f, true, false);
	//Collider* collider2 = physics->addSphereCollider(temp2, 0.5f, 0.4f, true, false);
	//Collider* collider3 = physics->addSphereCollider(temp3, 0.5f, 0.7f, true, false);
	//Collider* collider4 = physics->addSphereCollider(temp4, 0.5f, 0.5f, true, false);
	//Collider* collider5 = physics->addSphereCollider(temp5, 0.5f, 0.7f, true, false);
	
	//collider->ApplyForce({ 0.5f,0,0.1f });
	//collider1->ApplyForce({ -1.5f,0,0.0f });
	//collider1->ApplyAngularForce({ -0.5f,0.0f,0.0f });
	//collider2->ApplyForce({ 1.5f,0,0.0f });
	//collider3->ApplyForce({ -0.9f,0,0.1f });
	//collider4->ApplyForce({ 1.0f,0,-0.1f });
	//collider5->ApplyForce({ -1.4f,0,0.1f });
	for (int i = 0; i < 99; i++)
	{
		Entity* temp = new Entity(mesh, simpleMaterial);
		entityVector[i] = temp;
		temp->SetTranslation(rand()%5, rand() % 5 + 2 * i, rand() % 5);
		Collider* collider = physics->addSphereCollider(temp, 0.5f, 1.0f, true, false);
		collider->ApplyForce({ static_cast <float> (rand()) / static_cast <float> (RAND_MAX),  static_cast <float> (rand()) / static_cast <float> (RAND_MAX),  static_cast <float> (rand()) / static_cast <float> (RAND_MAX) });
	}
	entityVector[99] = terrain;
	//entityVector[100] = obstacle;
	Collider* collider6 = physics->addBoxCollider(terrain, XMFLOAT3{ 100,1,100 }, 1.0f, false, true);
	//Collider* collider7 = physics->addBoxCollider(obstacle, XMFLOAT3{ 3,3,3 }, 1.0f, false, true);
}

void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	XMMATRIX projection = camera->UpdateProjection((float)width / height);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(projection)); 
}

void Game::Update(float deltaTime, float totalTime) {
	if (deltaTime < 0.5f) {
		physics->CollisionsDetection(0, physics->NumCoolidersHandled, deltaTime, totalTime);
	}
	
	if (GetAsyncKeyState(VK_ESCAPE)) Quit();

	XMMATRIX view = camera->Update(deltaTime);
	
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Game::Draw(float deltaTime, float totalTime) {
	//backgroud color
	const float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//-set backgroud color
	//-clear depth buffer
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	
	//-------------------------------------

	for (int countOfEntity = 0; countOfEntity < entityVector.size(); countOfEntity++) {
		entityVector[countOfEntity]->SetWorldMatrix();
		entityVector[countOfEntity]->PrepareMatrix(viewMatrix, projectionMatrix);
		//entityVector[countOfEntity]->SetPointLight(pointLight, "pointLight");
		entityVector[countOfEntity]->SetLight(directionalLight, "light");
		//entityVector[countOfEntity]->SetLight(directionalLight1, "light1");
		//entityVector[countOfEntity]->SetSpotLight(spotLight, "spotLight");
		entityVector[countOfEntity]->SetTexture("diffuseTexture", "basicSampler");
		entityVector[countOfEntity]->SetNormalMap("normalMap");
		entityVector[countOfEntity]->CopyAllBufferData();
		entityVector[countOfEntity]->SetShader();
		//set vertex buffer and index buffer inside entity class
		entityVector[countOfEntity]->Draw(context);
	}

	//-------------------------------------

	//End of rendering one frame
	swapChain->Present(0, 0);

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
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
		camera->SetRotationX((y - prevMousePos.y) * 0.001f);
		camera->SetRotationY((x - prevMousePos.x) * 0.001f);
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

