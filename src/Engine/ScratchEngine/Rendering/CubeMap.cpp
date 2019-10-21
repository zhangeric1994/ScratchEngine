#include "CubeMap.h"


ScratchEngine::Rendering::CubeMap::CubeMap() {
	//rasterizer desc
	cubeRD = {};
	cubeRD.CullMode = D3D11_CULL_FRONT;
	cubeRD.FillMode = D3D11_FILL_SOLID;
	cubeRD.DepthClipEnable = true;

	//depth stencil desc
	cubeDSD = {};
	cubeDSD.DepthEnable = true;
	cubeDSD.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	cubeDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	//shader for cube
	cubeVS = 0;
	cubePS = 0;

	//rasterizer state & depth stencil state & shader resource view
	cubeRS = 0;
	cubeDSS = 0;
	cubeSRV = 0;

	//sampler
	sampler = 0;

	//buffer
	vertexBuffer = 0;
	indicesNum = 0;
	indexBuffer = 0;
}

ScratchEngine::Rendering::CubeMap::~CubeMap() {
	if (cubeVS) delete cubeVS;
	if (cubePS) delete cubePS;

	if (cubeRS) cubeRS->Release();
	if (cubeDSS) cubeDSS->Release();
	if (cubeSRV) cubeSRV->Release();
}

bool ScratchEngine::Rendering::CubeMap::setUp(ID3D11Device2* device) {
	bool isOk = false;

	isOk = device->CreateRasterizerState(&cubeRD, &cubeRS);
	isOk = device->CreateDepthStencilState(&cubeDSD, &cubeDSS);

	return isOk;
}

void ScratchEngine::Rendering::CubeMap::setSampler(ID3D11SamplerState* _sampler) {
	sampler = _sampler;
}

void ScratchEngine::Rendering::CubeMap::setMesh(Mesh * _cubeMesh) {
	vertexBuffer = _cubeMesh->GetVertexBuffer();
	indexBuffer = _cubeMesh->GetIndexBuffer();
	indicesNum = _cubeMesh->GetIndexCount();
}

void ScratchEngine::Rendering::CubeMap::setVS(ID3D11Device2* device, ID3D11DeviceContext2* context, LPCWSTR path) {
	cubeVS = new SimpleVertexShader(device, context);
	cubeVS->LoadShaderFile(path);
}

void ScratchEngine::Rendering::CubeMap::setPS(ID3D11Device2* device, ID3D11DeviceContext2* context, LPCWSTR path) {
	cubePS = new SimplePixelShader(device, context);
	cubePS->LoadShaderFile(path);
}

void ScratchEngine::Rendering::CubeMap::setSRV(ID3D11Device2* device, ID3D11DeviceContext2* context, const wchar_t* path) {
	CreateDDSTextureFromFile(device, context, path, 0, &cubeSRV);
	bool i = true;
}

ID3D11SamplerState * ScratchEngine::Rendering::CubeMap::getSampler() {
	return sampler;
}

SimpleVertexShader * ScratchEngine::Rendering::CubeMap::getVS() {
	return cubeVS;
}

SimplePixelShader * ScratchEngine::Rendering::CubeMap::getPS() {
	return cubePS;
}

ID3D11Buffer * ScratchEngine::Rendering::CubeMap::getVB() {
	return vertexBuffer;
}

ID3D11Buffer * ScratchEngine::Rendering::CubeMap::getIB() {
	return indexBuffer;
}

int ScratchEngine::Rendering::CubeMap::getIndexCount() {
	return indicesNum;
}

ID3D11RasterizerState * ScratchEngine::Rendering::CubeMap::getRS() {
	return cubeRS;
}

ID3D11DepthStencilState * ScratchEngine::Rendering::CubeMap::getDSS() {
	return cubeDSS;
}

ID3D11ShaderResourceView * ScratchEngine::Rendering::CubeMap::getSRV() {
	return cubeSRV;
}
