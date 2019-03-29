#ifndef GLOBAL_H
#define GLOBAL_H
#pragma once

#include "../Common/Typedefs.h"

namespace ScratchEngine
{
	struct Global
	{
	private:
		static f32 screenRatio;
		
	public:
		static f32 GetScreenRatio();
	};
}
#endif