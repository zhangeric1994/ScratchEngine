#include "Animator.h"

using namespace ScratchEngine;
using namespace std;

namespace ScratchEngine
{
	namespace Animation
	{
		class AnimationEngine
		{

			friend class Animator;

		public:
			//static AnimationEngine * singleton;

			//static AnimationEngine* GetSingleton();
			//static void Initialize();
			void UpdateAnimators(float dt);

		private:
			Animator * animatorList;


			AnimationEngine();


			void AddAnimator(Animator* animator);

			void RemoveAnimator(Animator* animator);
		};
	}
}