#include "Game.h"
#include <string>

using namespace DirectX;

Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true) {
	vertexShader = 0;
	pixelShader = 0;

	mesh = 0;

	entityVector.resize(1);
	for (int countOfVector = 0; countOfVector < entityVector.size(); countOfVector++)
		entityVector[countOfVector] = NULL;

	camera = new Camera();

	simpleMaterial = NULL;

	directionalLight.AmbientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	directionalLight.DiffuseColor = XMFLOAT4(0, 0, 0.5, 1);
	directionalLight.Direction = XMFLOAT3(1, -1, 0);

	pointLight.AmbientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	pointLight.DiffuseColor = XMFLOAT4(0.0f, 0.0f, 0.7f, 1.0f);
	pointLight.Position = XMFLOAT3(0, 0.0f, -1.0f);
}

Game::~Game() {
	for (int countOfEntity = 0; countOfEntity < entityVector.size(); countOfEntity++) {
		delete entityVector[countOfEntity];
	}

	delete vertexShader;
	delete pixelShader;
	if (mesh) delete mesh;
	delete camera;
	if (simpleMaterial) delete simpleMaterial;
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
	
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	XMMATRIX projection = camera->UpdateProjection((float)width / height);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(projection)); // Transpose for HLSL!
}

void Game::CreateBasicGeometry() {
	simpleMaterial = new Material(vertexShader, pixelShader, 0, 0);

	char* filename = (char*)"../Assets/Models/cone.obj";
	//char* filename = (char*)"D://GitRepo/scratch-engine/src/Editor/Assets/Models/cone.obj";

	mesh = new Mesh(device, filename);

	entityVector[0] = new Entity(mesh, simpleMaterial);


	/*char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	std::string path = std::string(buffer).substr(0, pos);*/

}

void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	XMMATRIX projection = camera->UpdateProjection((float)width / height);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(projection)); // Transpose for HLSL!
}

void Game::Update(float deltaTime, float totalTime) {
	if (GetAsyncKeyState(VK_ESCAPE)) Quit();

	XMMATRIX view = camera->Update();
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Game::Draw(float deltaTime, float totalTime) {
	//backgroud color
	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.6f };

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
		entityVector[countOfEntity]->PrepareMatrix(viewMatrix, projectionMatrix);
		entityVector[countOfEntity]->SetPointLight(pointLight, "pointLight");
		entityVector[countOfEntity]->SetLight(directionalLight, "light");
		entityVector[countOfEntity]->CopyAllBufferData();
		entityVector[countOfEntity]->SetShader();

		//set vertex buffer and index buffer inside entity class
		entityVector[countOfEntity]->Draw(context);
	}


	//-------------------------------------


	//End of rendering one frame
	swapChain->Present(0, 0);
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

