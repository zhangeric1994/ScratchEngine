#include "Global.h"

ScratchEngine::f32 ScratchEngine::Global::screenRatio = -1;

ScratchEngine::f32 ScratchEngine::Global::GetScreenRatio()
{
	return screenRatio;
}

void ScratchEngine::Global::SetScreenRatio(f32 value)
{
	screenRatio = value;
}
