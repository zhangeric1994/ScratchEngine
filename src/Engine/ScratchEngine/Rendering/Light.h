#ifndef LIGHT_H
#define LIGHT_H
#pragma once

#include <DirectXMath.h>

#include "../Core/GameComponent.h"

#include "Mesh.h"

using namespace DirectX;
using namespace ScratchEngine::Rendering;

namespace ScratchEngine
{
	enum LightType : i32
	{
		Directional = 0,
		Point = 1,
		Spot = 2,
	};

	class __declspec(dllexport) Light : public GameComponent
	{
		friend class RenderingEngine;


	protected:
		XMVECTOR ambientColor;
		XMVECTOR diffuseColor;

		LightType type;

		Light* next;
		Light* previous;

		Light(LightType type);
		Light(LightType type, XMVECTOR ambientColor, XMVECTOR diffuseColor);
		~Light();


	public:
		XMVECTOR GetPosition();
		XMVECTOR GetAmbientColor();
		XMVECTOR GetDiffuseColor();

		void SetAmbientColor(f32 r, f32 g, f32 b);
		void SetDiffuseColor(f32 r, f32 g, f32 b);
	};

	class __declspec(dllexport) DirectionalLight : public Light
	{
	public:
		DirectionalLight();
		DirectionalLight(XMVECTOR ambientColor, XMVECTOR diffuseColor);

		XMVECTOR GetDirection();
	};

	class __declspec(dllexport) PointLight : public Light
	{
	private:
		f32 range;


	public:
		PointLight();
		PointLight(XMVECTOR ambientColor, XMVECTOR diffuseColor);
	};

	class __declspec(dllexport) SpotLight : public Light
	{
	private:
		f32 range;
		f32 angle;


	public:
		SpotLight();
		SpotLight(XMVECTOR ambientColor, XMVECTOR diffuseColor, f32 angle);
	};
}
#endif