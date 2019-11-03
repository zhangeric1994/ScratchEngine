#pragma once

#include <d3d11_2.h>
#include <DirectXMath.h>

#include "../Common/Typedefs.h"

#include "SimpleShader.h"
#include "GFSDK_SSAO.h"

using namespace DirectX;
using namespace ScratchEngine;


namespace ScratchEngine
{
	namespace Rendering
	{
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
		class Texture2D;
		class TextureCube;
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
#define MAX_NUM_BONES_PER_MODEL 128
#define DEFAULT_MAX_NUM_MATERIALS 512
#define DEFAULT_MAX_NUM_MESHES 8192
	}
}


using namespace ScratchEngine::Rendering;