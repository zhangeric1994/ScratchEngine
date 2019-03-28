#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera {
public:
	Camera();
	Camera(XMFLOAT3 _position, XMFLOAT3 _direction);

	void SetRotationX(float _directionX);
	void SetRotationY(float _directionY);

	XMMATRIX Update(float deltaTime);
	XMMATRIX UpdateProjection(float aspectRatio);

	XMFLOAT3 getPosition();

private:
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float	 rotationX;
	float	 rotationY;
};