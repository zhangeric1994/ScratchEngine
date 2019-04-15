#ifndef GLOBAL_H
#define GLOBAL_H
#pragma once

#include "../Common/Typedefs.h"

namespace ScratchEngine
{
	struct __declspec(dllexport) Global
	{
	private:
		static f32 screenRatio;
		
	public:
		static f32 GetScreenRatio();
		static void SetScreenRatio(f32 value);
	};
}
#endif