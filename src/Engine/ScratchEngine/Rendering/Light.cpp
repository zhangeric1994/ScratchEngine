#include "../Core/GameObject.h"

#include "Light.h"
#include "RenderingEngine.h"

ScratchEngine::Light::Light(LightType type) : Light(type, XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f), XMVectorSet(0.6f, 0.6f, 0.0f, 1.0f))
{
}

ScratchEngine::Light::Light(LightType type, XMVECTOR ambientColor, XMVECTOR diffuseColor)
{
	this->type = type;
	this->ambientColor = ambientColor;
	this->diffuseColor = diffuseColor;

	this->next = nullptr;
	this->previous = nullptr;

	RenderingEngine::GetSingleton()->AddLight(this);
}

ScratchEngine::Light::~Light()
{
	RenderingEngine::GetSingleton()->RemoveLight(this);
}

XMVECTOR ScratchEngine::Light::GetPosition()
{
	return gameObject->GetPosition();
}

XMVECTOR ScratchEngine::Light::GetAmbientColor()
{
	return ambientColor;
}

XMVECTOR ScratchEngine::Light::GetDiffuseColor()
{
	return diffuseColor;
}

void ScratchEngine::Light::SetAmbientColor(f32 r, f32 g, f32 b)
{
	ambientColor = XMVectorSet(r, g, b, 0.0f);
}

void ScratchEngine::Light::SetDiffuseColor(f32 r, f32 g, f32 b)
{
	diffuseColor = XMVectorSet(r, g, b, 0.0f);
}


ScratchEngine::DirectionalLight::DirectionalLight() : Light(Directional)
{
}

ScratchEngine::DirectionalLight::DirectionalLight(XMVECTOR ambientColor, XMVECTOR diffuseColor) : Light(Directional, ambientColor, diffuseColor)
{
}

XMVECTOR ScratchEngine::DirectionalLight::GetDirection()
{
	return XMVector3Rotate(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), gameObject->GetRotation());
}


ScratchEngine::PointLight::PointLight() : Light(Point)
{
}

ScratchEngine::PointLight::PointLight(XMVECTOR ambientColor, XMVECTOR diffuseColor) : Light(Point, ambientColor, diffuseColor)
{
}


ScratchEngine::SpotLight::SpotLight() : Light(Spot)
{
	this->angle = 45.0f;
}

ScratchEngine::SpotLight::SpotLight(XMVECTOR ambientColor, XMVECTOR diffuseColor, f32 angle) : Light(Spot, ambientColor, diffuseColor)
{
	this->angle = angle;
}
