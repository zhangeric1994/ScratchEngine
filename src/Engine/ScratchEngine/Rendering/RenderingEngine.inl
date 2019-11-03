#ifndef RENDERING_ENGINE_INL
#define RENDERING_ENGINE_INL
#pragma once

#include "prerequisites.h"


inline RenderingEngine* ScratchEngine::Rendering::RenderingEngine::GetSingleton()
{
	return singleton;
}

inline void ScratchEngine::Rendering::RenderingEngine::Initialize(ID3D11Device2* device, ID3D11DeviceContext2* deviceContext)
{
	RenderingEngineConfig config;
	config.device = device;
	config.deviceContext = deviceContext;

	singleton = new RenderingEngine(config);


	singleton->sphereMesh = new Mesh(device, (char*)"../Assets/Models/sphere.obj");
	singleton->cubeMesh = new Mesh(device, (char*)"../Assets/Models/cube.obj");
}

inline void ScratchEngine::Rendering::RenderingEngine::Initialize(RenderingEngineConfig config)
{
	if (!singleton)
		singleton = new RenderingEngine(config);
}

inline void ScratchEngine::Rendering::RenderingEngine::Terminate()
{
	if (singleton)
		delete singleton;
}

#endif