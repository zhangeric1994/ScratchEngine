#pragma once

#include <Windows.h>

class Game
{
private:
	Game();

public:
	Game(HINSTANCE hInstance);
	~Game();

	HRESULT InitializeWindow();
	HRESULT InitializeDirectX();

	int Run();
};