#ifndef LIGHT_H
#define LIGHT_H
#pragma once

#include "../header.h"

#include "../Rendering/LightSource.h"
#include "../Rendering/Texture.h"

#include "DXCore.h"
#include "GameComponent.h"

using namespace DirectX;
using namespace ScratchEngine::Rendering;


namespace ScratchEngine
{
	class __declspec(dllexport) Light : public GameComponent
	{
		friend class Scene;


	protected:
		XMVECTOR color;
		LightType type;
		f32 intensity;

		Shadow* shadow;

		Light* next;
		Light* previous;


		Light(LightType type);
		Light(LightType type, XMVECTOR color, f32 intensity = 1);
		~Light();


	public:
		XMVECTOR GetColor();
		bool DoCastShadow();

		void SetColor(f32 r, f32 g, f32 b);
		void SetIntensity(f32 value);
		virtual void EnableShadowCasting() = 0;
		virtual void DisableShadowCasting() = 0;
	};


	class __declspec(dllexport) DirectionalLight : public Light
	{
		friend class Scene;


	public:
		DirectionalLight();
		DirectionalLight(XMVECTOR color, f32 intensity = 1);


		void EnableShadowCasting();
		void EnableShadowCasting(int numCascade);

		void DisableShadowCasting();
	};


	class __declspec(dllexport) PointLight : public Light
	{
		friend class Scene;


	private:
		f32 range;


	public:
		PointLight();
		PointLight(XMVECTOR color, f32 intensity = 1, f32 range = 1);


		void EnableShadowCasting();
		void DisableShadowCasting();
	};


	class __declspec(dllexport) SpotLight : public Light
	{
		friend class Scene;


	private:
		f32 range;
		f32 angle;


	public:
		SpotLight();
		SpotLight(XMVECTOR color, f32 angle);


		void EnableShadowCasting();
		void DisableShadowCasting();
	};


	class __declspec(dllexport) AmbientLight : public Light
	{
		friend class Scene;


	public:
		AmbientLight();
		AmbientLight(XMVECTOR color);


		void EnableShadowCasting();
		void DisableShadowCasting();
	};


	inline XMVECTOR Light::GetColor()
	{
		return color;
	}

	inline void Light::SetColor(f32 r, f32 g, f32 b)
	{
		color = XMVectorSet(r, g, b, 0.0f);
	}

	inline void Light::SetIntensity(f32 value)
	{
		intensity = value;
	}

	inline bool Light::DoCastShadow()
	{
		return shadow != nullptr;
	}


	inline void DirectionalLight::EnableShadowCasting(int numCascade)
	{
		if (!shadow)
			shadow = new Shadow(2048, LightType::DIRECTIONAL, numCascade);
	}

	inline void DirectionalLight::EnableShadowCasting()
	{
		EnableShadowCasting(1);
	}

	inline void DirectionalLight::DisableShadowCasting()
	{
		if (shadow)
			delete shadow;

		shadow = nullptr;
	}


	inline void PointLight::EnableShadowCasting()
	{
		if (!shadow)
			shadow = new Shadow(2048, LightType::POINT);
	}

	inline void PointLight::DisableShadowCasting()
	{
		if (shadow)
			delete shadow;

		shadow = nullptr;
	}


	inline void SpotLight::EnableShadowCasting()
	{
	}

	inline void SpotLight::DisableShadowCasting()
	{
		if (shadow)
			delete shadow;

		shadow = nullptr;
	}


	inline void AmbientLight::EnableShadowCasting()
	{
		if (!shadow)
			shadow = new Shadow(DXCore::GetSingleton()->GetWindowWidth(), DXCore::GetSingleton()->GetWindowHeight(), LightType::AMBIENT);
	}

	inline void AmbientLight::DisableShadowCasting()
	{
		if (shadow)
			delete shadow;

		shadow = nullptr;
	}
}
#endif