#pragma once

#include "DXCore.h"

class Game : public DXCore
{
private:
	Game();

public:
	Game(HINSTANCE hInstance, char* name);
	~Game();

	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
};