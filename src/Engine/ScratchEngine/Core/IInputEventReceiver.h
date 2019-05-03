#ifndef IINPUTEVENTCALLBACK_H
#define IINPUTEVENTCALLBACK_H
#pragma once

#include "../Common/Typedefs.h"


namespace ScratchEngine
{
	struct __declspec(dllexport) IInputEventReceiver
	{
	private:
		virtual void OnKeyPressed(i32 keyCode) = 0;
		virtual void OnKeyDown(i32 keyCode) = 0;
		virtual void OnKeyUp(i32 keyCode) = 0;
		virtual void OnMouseMovement(i32 x, i32 y) = 0;
	};
}
#endif