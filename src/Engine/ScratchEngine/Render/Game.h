#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <vector>

namespace ScratchEngine {
	namespace Render {
		class Game
			: public DXCore
		{
		public:
			Game(HINSTANCE hInstance, char* name);
			~Game();

			void Init();
			void Update(float deltaTime, float totalTime);
			void Draw(float deltaTime, float totalTime);

		private:


		};
	}

}

