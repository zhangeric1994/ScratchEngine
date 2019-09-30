#include "RenderingEngine.h"
#include "Texture.h"

using namespace DirectX;


ScratchEngine::Rendering::Texture::Texture()
{
	shaderResourceView = nullptr;
	samplerState = nullptr;
}

ScratchEngine::Rendering::Texture::Texture(wchar_t* _filepath, D3D11_TEXTURE_ADDRESS_MODE _addressMode, D3D11_FILTER _filterMode, f32 _maxLOD)
{
	RenderingEngine* renderingEngine = RenderingEngine::GetSingleton();
	ID3D11Device* device = renderingEngine->device;
	ID3D11DeviceContext* deviceContext = renderingEngine->deviceContext;


	ID3D11Resource* texture = nullptr;
	CreateWICTextureFromFile(device, deviceContext, _filepath, &texture, &shaderResourceView);


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = _addressMode;
	samplerDesc.AddressV = _addressMode;
	samplerDesc.AddressW = _addressMode;
	samplerDesc.Filter = _filterMode;
	samplerDesc.MaxLOD = _maxLOD;
	device->CreateSamplerState(&samplerDesc, &samplerState);


	texture->Release();
}

ScratchEngine::Rendering::Texture::~Texture()
{
	shaderResourceView->Release();
	samplerState->Release();
}


ScratchEngine::Rendering::Shadow::Shadow(u32 size) : Shadow(size, size)
{
}

ScratchEngine::Rendering::Shadow::Shadow(u32 width, u32 height)
{
	this->width = width;
	this->height = height;


	ID3D11Device* device = RenderingEngine::GetSingleton()->device;


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&samplerDesc, &samplerState);


	ID3D11Texture2D* texture = nullptr;


	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateTexture2D(&textureDesc, nullptr, &texture);


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(texture, &dsvDesc, &depthStencilView);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture, &srvDesc, &shaderResourceView);


	texture->Release();
}

ScratchEngine::Rendering::Shadow::~Shadow()
{
	renderTargetView->Release();
	depthStencilView->Release();
}
