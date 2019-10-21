#ifndef CAMERA_H
#define CAMERA_H
#pragma once

#include "../Rendering/Mesh.h"
#include "../Rendering/Texture.h"
#include "../Rendering/Viewer.h"

#include "GameComponent.h"

using namespace ScratchEngine::Rendering;


namespace ScratchEngine
{
	class __declspec(dllexport)  Camera : public GameComponent
	{
		friend class Scene;


	private:
		f32 fov;
		f32 nearZ;
		f32 farZ;
		i32 viewer;

		Camera* next;
		Camera* previous;


	public:
		Camera();
		~Camera();
	};
}
#endif // !CAMERA_H