
#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight{
	XMFLOAT3 AmbientColor;
	float	 CameraX;
	XMFLOAT3 DiffuseColor;
	float	 CameraY;
	XMFLOAT3 Direction;
	float	 CameraZ;
};

struct PointLight {
	XMFLOAT3 AmbientColor;
	float	 CameraX;
	XMFLOAT3 DiffuseColor;
	float	 CameraY;
	XMFLOAT3 Position;
	float	 CameraZ;
};

struct SpotLight {
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Position;
	float	 Cone;
	XMFLOAT3 Direction;
	float	 Range;
};