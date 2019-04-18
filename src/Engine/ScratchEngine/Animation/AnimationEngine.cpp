#include "AnimationEngine.h"

//AnimationEngine * ScratchEngine::Animation::AnimationEngine::GetSingleton()
//{
//	if (!singleton)
//		Initialize();
//
//	return singleton;
//}
//
//void ScratchEngine::Animation::AnimationEngine::Initialize()
//{
//	singleton = new AnimationEngine();
//}

ScratchEngine::Animation::AnimationEngine::AnimationEngine()
{
	animatorList = nullptr;
}

void ScratchEngine::Animation::AnimationEngine::AddAnimator(Animator * animator)
{
	animator->next = animatorList;
	animator->previous = nullptr;

	if (animatorList)
		animatorList->previous = animator;

	animatorList = animator;
}

void ScratchEngine::Animation::AnimationEngine::RemoveAnimator(Animator * animator)
{

	if (!animatorList)
		return;

	Animator* next = animator->next;
	Animator* previous = animator->previous;

	if (previous)
		previous->next = next;
	else
		animatorList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Animation::AnimationEngine::UpdateAnimators(float dt)
{

	for (Animator * animator = animatorList; animator; animator = animator->next)
	{
		animator->Update(dt);
	}

}
