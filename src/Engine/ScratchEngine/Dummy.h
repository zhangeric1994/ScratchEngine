#pragma once

#include "Core/GameComponent.h"
#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Rendering/Material.h"


namespace ScratchEngine
{
	class HitReaction : public GameComponent
	{
	private:
		Material* onHitMaterial;
		Material* normalMaterial;


	public:
		HitReaction(Material* material1, Material* material2);


	private:
		void OnBeginOverlapping(GameObject* other);
		void OnEndOverlapping(GameObject* other);
	};
}


inline ScratchEngine::HitReaction::HitReaction(Material* materialA, Material* materialB)
{
	onHitMaterial = materialA;
	normalMaterial = materialB;
}

inline void ScratchEngine::HitReaction::OnBeginOverlapping(GameObject* other)
{
	gameObject->GetComponent<Renderer>()->SetMaterial(onHitMaterial);
}

inline void ScratchEngine::HitReaction::OnEndOverlapping(GameObject* other)
{
	gameObject->GetComponent<Renderer>()->SetMaterial(normalMaterial);
}
