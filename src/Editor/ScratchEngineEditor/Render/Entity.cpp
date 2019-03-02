#include "Entity.h"

Entity::Entity(Mesh* _mesh, Material* _material) {
	mesh = _mesh;
	material = _material;

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));

	individualPositon = XMFLOAT3(0, 0, 0);
	scaleVector = XMFLOAT3(1, 1, 1);
	quaternion = XMFLOAT3(0, 0, 0);

	isChanged = false;
}

void Entity::SetTranslation(float _x, float _y, float _z) {
	individualPositon = XMFLOAT3(_x, _y, _z);
	isChanged = true;
}

void Entity::SetRotation(float _x, float _y, float _z) {
	quaternion = XMFLOAT3(_x, _y, _z);
	isChanged = true;
}

void Entity::SetScale(float _x, float _y, float _z) {
	scaleVector = XMFLOAT3(_x, _y, _z);
	isChanged = true;
}

void Entity::SetWorldMatrix() {
	//translation
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&individualPositon));

	//rotation
	XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&quaternion));
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuaternion);

	//scale
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scaleVector));

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix));

	isChanged = false;
}

XMFLOAT4X4 Entity::GetWorldMatrix() {
	return worldMatrix;
}

Mesh* Entity::GetMesh() {
	return mesh;
}

void Entity::Draw(ID3D11DeviceContext* context) {
	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(
		mesh->GetIndexCount(),
		0,    
		0);  
	
}

void Entity::PrepareMatrix(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix) {
	//set up world view projection matrix for vertex shader
	material->getVertexShader()->SetMatrix4x4("world", worldMatrix);
	material->getVertexShader()->SetMatrix4x4("view", viewMatrix);
	material->getVertexShader()->SetMatrix4x4("projection", projectionMatrix);
}

void Entity::CopyAllBufferData() {
	material->getVertexShader()->CopyAllBufferData();
	material->getPixelShader()->CopyAllBufferData();
}

void Entity::SetShader() {
	material->getVertexShader()->SetShader();
	material->getPixelShader()->SetShader();
}

void Entity::SetLight(DirectionalLight directionalLight, char* bufferName) {
	material->getPixelShader()->SetData(bufferName, &directionalLight, sizeof(DirectionalLight));
}

void Entity::SetTexture(char* textureName, char* samplerName) {
	material->getPixelShader()->SetShaderResourceView(textureName, material->getTexture());
	material->getPixelShader()->SetSamplerState(samplerName, material->getSampler());
}

bool Entity::isUpdated() {
	return isChanged;
}