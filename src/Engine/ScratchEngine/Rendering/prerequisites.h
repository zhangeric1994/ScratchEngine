#pragma once

#include <DirectXMath.h>

#include "SimpleShader.h"

#include "../Common/Typedefs.h"

using namespace DirectX;


namespace ScratchEngine
{
	namespace Rendering
	{
		// CubeMap.h
		struct CubeMap;

		// LightSource.h
		enum class LightType;
		struct LightSource;

		// Material.h
		struct Material;

		// Mesh.h
		struct Mesh;

		// Renderable.h
		struct Renderable;

		// RenderingEngine.h
		struct RenderingEngineConfig;
		class RenderingEngine;

		// ShadowMap.h
		struct ShadowMap;

		// Texture.h
		class Texture;
		class Shadow;

		// Vertex.h
		struct Vertex;

		// Viewer.h
		struct Viewer;


		struct CSMConfig
		{
			LightSource* light;
			Viewer* viewer;
			D3D11_VIEWPORT* viewport;
			ID3D11DepthStencilView* depthStencilView;
			float nearZ;
			float farZ;
			float fov;
			float aspectRatio;
			int numCascades;
			float selectionFactor;
			float powerExponent;
		};


#define NUM_BONES_PER_VEREX 4
	}
}