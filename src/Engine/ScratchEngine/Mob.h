#pragma once

#include "Core/GameComponent.h"
#include "Core/GameObject.h"
#include "Core/Renderer.h"
#include "Rendering/Material.h"
#include "Rendering/Model.h"


namespace ScratchEngine
{
	class Mob : public GameComponent
	{
	private:
		Model* model;


	public:
		Mob(Model* model);


	private:
		void OnBeginOverlapping(GameObject* other);
	};
}


inline ScratchEngine::Mob::Mob(Model* model)
{
	this->model = model;
}

inline void ScratchEngine::Mob::OnBeginOverlapping(GameObject* other)
{
	model->anim->SetAnimationIndex(rand() % 2 + 4, false);
}