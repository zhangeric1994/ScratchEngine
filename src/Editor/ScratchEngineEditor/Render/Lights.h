
#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Direction;
};

struct PointLight {
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Position;
};

struct SpotLight {
	XMFLOAT4 AmbientColor;
	XMFLOAT4 DiffuseColor;
	XMFLOAT3 Position;
	XMFLOAT3 Direction;
	float	 Range;
};