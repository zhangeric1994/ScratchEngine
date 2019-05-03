#pragma once

#include "Core/GameComponent.h"
#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Rendering/Material.h"


namespace ScratchEngine
{
	class Dummy : public GameComponent
	{
	private:
		Material* onHitMaterial;
		Material* normalMaterial;


	public:
		Dummy(Material* material1, Material* material2);


	private:
		void OnBeginOverlapping(GameObject* other);
		void OnEndOverlapping(GameObject* other);
	};
}


inline ScratchEngine::Dummy::Dummy(Material* materialA, Material* materialB)
{
	onHitMaterial = materialA;
	normalMaterial = materialB;
}

inline void ScratchEngine::Dummy::OnBeginOverlapping(GameObject* other)
{
	gameObject->GetComponent<Renderer>()->SetMaterial(onHitMaterial);
}

inline void ScratchEngine::Dummy::OnEndOverlapping(GameObject* other)
{
	gameObject->GetComponent<Renderer>()->SetMaterial(normalMaterial);
}
