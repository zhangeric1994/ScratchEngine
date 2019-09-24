#ifndef LIGHT_H
#define LIGHT_H
#pragma once

#include <DirectXMath.h>

#include "../Rendering/LightSource.h"
#include "../Rendering/Texture.h"

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
		RenderTexture* shadowMap;

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
		return shadowMap != nullptr;
	}


	inline void DirectionalLight::EnableShadowCasting()
	{
		if (!shadowMap)
			shadowMap = new RenderTexture(2048);
	}

	inline void DirectionalLight::DisableShadowCasting()
	{
		if (shadowMap)
			delete shadowMap;
	}


	inline void PointLight::EnableShadowCasting()
	{
		if (!shadowMap)
			shadowMap = new RenderTexture(2048);
	}

	inline void PointLight::DisableShadowCasting()
	{
		if (shadowMap)
			delete shadowMap;
	}
}
#endif