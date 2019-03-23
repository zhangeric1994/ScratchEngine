#include "Render/Game.h"

using namespace DirectX;

Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1280, 720, true) {

}

Game::~Game() {

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

