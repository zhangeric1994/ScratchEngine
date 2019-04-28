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
		RenderTexture* shadowMap;

		LightType type;

		Light* next;
		Light* previous;

		Light(LightType type);
		Light(LightType type, XMVECTOR ambientColor, XMVECTOR diffuseColor);
		~Light();


	public:
		XMVECTOR GetAmbientColor();
		XMVECTOR GetDiffuseColor();
		bool DoCastShadow();

		void SetAmbientColor(f32 r, f32 g, f32 b);
		void SetDiffuseColor(f32 r, f32 g, f32 b);
		virtual void EnableShadowCasting() = 0;
		virtual void DisableShadowCasting() = 0;
	};

	class __declspec(dllexport) DirectionalLight : public Light
	{
	public:
		DirectionalLight();
		DirectionalLight(XMVECTOR ambientColor, XMVECTOR diffuseColor);


		void EnableShadowCasting();
		void DisableShadowCasting();
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


	inline XMVECTOR Light::GetAmbientColor()
	{
		return ambientColor;
	}

	inline XMVECTOR Light::GetDiffuseColor()
	{
		return diffuseColor;
	}

	inline void Light::SetAmbientColor(f32 r, f32 g, f32 b)
	{
		ambientColor = XMVectorSet(r, g, b, 0.0f);
	}

	inline void Light::SetDiffuseColor(f32 r, f32 g, f32 b)
	{
		diffuseColor = XMVectorSet(r, g, b, 0.0f);
	}

	inline bool Light::DoCastShadow()
	{
		return shadowMap != nullptr;
	}


	inline void DirectionalLight::EnableShadowCasting()
	{
		if (!shadowMap)
		{
			shadowMap = new RenderTexture(2048);
		}
	}

	inline void DirectionalLight::DisableShadowCasting()
	{
		if (shadowMap)
			delete shadowMap;
	}
}
#endif