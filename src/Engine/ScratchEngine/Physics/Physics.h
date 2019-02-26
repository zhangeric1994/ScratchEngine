#pragma once

namespace ScratchEngine
{
	namespace Physics
	{
		class Collider
		{
		public:
			__declspec(dllexport) const char* ToStringA();

		protected:
			const char* ToStringB();

		private:
			const char* ToStringC();
		};
	}
}