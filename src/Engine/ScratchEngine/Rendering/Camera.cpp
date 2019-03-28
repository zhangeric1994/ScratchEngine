#include "Camera.h"
#include <Windows.h>

Camera::Camera() {
	position = XMFLOAT3(0, 0, -20);
	direction = XMFLOAT3(0, 0, 1);

	rotationX = 0.0f;
	rotationY = 0.0f;
}

Camera::Camera(XMFLOAT3 _position, XMFLOAT3 _direction) {
	position = _position;
	direction = _direction;

	rotationX = 0.0f;
	rotationY = 0.0f;
}

void Camera::SetRotationX(float _rotationX) {
	rotationX = _rotationX;
}

void Camera::SetRotationY(float _rotationY) {
	rotationY = _rotationY;
}

XMMATRIX Camera::Update() {
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { position.y += -0.0001f; }
	if (GetAsyncKeyState('X') & 0x8000) { position.y -= -0.0001f; }

	//rotation quaternion
	XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&XMFLOAT3(rotationX, rotationY, 0)));
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuaternion);

	//applying rotation quaternion to direction
	XMVECTOR directionVector = XMVector3Transform(XMLoadFloat3(&direction), rotationMatrix);
	XMStoreFloat3(&direction, directionVector);

	//speed needs to be set

	//move forward
	if (GetAsyncKeyState('W') & 0x8000) {
		position.x += direction.x * 0.001f;
		position.y += direction.y * 0.001f;
		position.z += direction.z * 0.001f;
	}

	//move backward
	if (GetAsyncKeyState('S') & 0x8000) {
		position.x -= direction.x * 0.001f;
		position.y -= direction.y * 0.001f;
		position.z -= direction.z * 0.001f;
	}

	XMFLOAT3 upDirection = XMFLOAT3(0, 1.0f, 0);

	//left direction
	XMVECTOR leftVector;
	XMFLOAT3 leftDirection;
	leftVector = XMVector3Cross(XMLoadFloat3(&upDirection), XMLoadFloat3(&direction));

	XMStoreFloat3(&leftDirection, leftVector);

	if (GetAsyncKeyState('A') & 0x8000) {
		position.x += leftDirection.x * 0.001f;
		position.y += leftDirection.y * 0.001f;
		position.z += leftDirection.z * 0.001f;
	}

	if (GetAsyncKeyState('D') & 0x8000) {
		position.x -= leftDirection.x * 0.001f;
		position.y -= leftDirection.y * 0.001f;
		position.z -= leftDirection.z * 0.001f;
	}

	//generate view matrix
	XMMATRIX viewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), directionVector, XMLoadFloat3(&upDirection));

	//reset rotation
	rotationX = 0.0f;
	rotationY = 0.0f;

	return viewMatrix;
}

XMMATRIX Camera::UpdateProjection(float aspectRatio) {
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)aspectRatio,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance

	return projectionMatrix;
}