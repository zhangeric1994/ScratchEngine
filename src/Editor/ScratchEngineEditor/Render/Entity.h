#pragma once
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Lights.h"

using namespace DirectX;

class Entity {
public:
	Entity(Mesh* _mesh, Material* _material);

	void SetTranslation(float _x, float _y, float _z);
	void SetScale(float _x, float _y, float _z);
	void SetRotation(float _x, float _y, float _z);
	void SetWorldMatrix();

	XMFLOAT4X4 GetWorldMatrix();
	Mesh* GetMesh();

	void Draw(ID3D11DeviceContext* context);
	void PrepareMatrix(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);
	void CopyAllBufferData();
	void SetShader();
	void SetLight(DirectionalLight directionalLight, char* bufferName);
	void SetTexture(char* textureName, char* samplerName);

	bool isUpdated();

private:
	XMFLOAT4X4 worldMatrix;

	XMFLOAT3 individualPositon;
	XMFLOAT3 scaleVector;
	XMFLOAT3 quaternion;

	Mesh* mesh;

	Material* material;

	bool isChanged;
};