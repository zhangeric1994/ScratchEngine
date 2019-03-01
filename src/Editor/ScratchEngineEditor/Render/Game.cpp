#include "Game.h"

using namespace DirectX;

Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true) {
	vertexShader = 0;
	pixelShader = 0;

	mesh = NULL;

	entityVector.resize(1);
	for (int countOfVector = 0; countOfVector < entityVector.size(); countOfVector++)
		entityVector[countOfVector] = NULL;

	camera = new Camera();

	simpleMaterial = NULL;

	directionalLight.AmbientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	directionalLight.DiffuseColor = XMFLOAT4(0, 0, 0.5, 1);
	directionalLight.Direction = XMFLOAT3(1, -1, 0);
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

}

void Game::Update(float deltaTime, float totalTime) {

}

void Game::Draw(float deltaTime, float totalTime) {
	//backgroud color
	const float color[4] = { 0.3f, 0.3f, 0.3f, 0.0f };


	//-set backgroud color
	//-clear depth buffer
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//-------------------------------------








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

