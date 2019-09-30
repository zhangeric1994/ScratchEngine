#pragma once

// SimpleShader.h
class SimpleVertexShader;
class SimplePixelShader;
class SimpleComputeShader;

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
	}
}