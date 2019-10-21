#include "LightSource.h"
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
	if (shaderResourceView)
		shaderResourceView->Release();

	if (samplerState)
		samplerState->Release();
}


ScratchEngine::Rendering::Shadow::Shadow(u32 size, LightType type, int numCascades) : Shadow(size, size, type, numCascades)
{
}

ScratchEngine::Rendering::Shadow::Shadow(u32 width, u32 height, LightType type, int numCascades)
{
	this->type = type;
	this->width = width;
	this->height = height;
	this->numCascades = numCascades;


	for (int i = 0; i < 6; ++i)
	{
		depthStencilViews[i] = nullptr;
		renderTargetViews[i] = nullptr;
	}


	ID3D11Device* device = RenderingEngine::GetSingleton()->device;


	D3D11_SAMPLER_DESC samplerDesc = {};
	D3D11_TEXTURE2D_DESC textureDesc = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ID3D11Texture2D* texture = nullptr;

	switch (type)
	{
		case LightType::DIRECTIONAL:
			samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.BorderColor[0] = 10;
			samplerDesc.BorderColor[1] = 10;
			samplerDesc.BorderColor[2] = 10;
			samplerDesc.BorderColor[3] = 10;
			device->CreateSamplerState(&samplerDesc, &samplerState);
			
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.ArraySize = numCascades;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			textureDesc.MipLevels = 1;
			textureDesc.MiscFlags = 0;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			device->CreateTexture2D(&textureDesc, nullptr, &texture);
			
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.ArraySize = 1;
			for (int i = 0; i < numCascades; ++i)
			{
				dsvDesc.Texture2DArray.FirstArraySlice = i;
				device->CreateDepthStencilView(texture, &dsvDesc, &depthStencilViews[i]);
			}
			
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.Texture2DArray.ArraySize = numCascades;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			device->CreateShaderResourceView(texture, &srvDesc, &shaderResourceView);

			texture->Release();

			break;


		case LightType::POINT:
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.ArraySize = 6;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			textureDesc.MipLevels = 1;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			device->CreateTexture2D(&textureDesc, nullptr, &texture);

			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.ArraySize = 1;
			for (int i = 0; i < 6; ++i)
			{
				dsvDesc.Texture2DArray.FirstArraySlice = i;
				device->CreateDepthStencilView(texture, &dsvDesc, &depthStencilViews[i]);
			}

			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.TextureCube.MipLevels = -1;
			srvDesc.TextureCube.MostDetailedMip = 0;
			device->CreateShaderResourceView(texture, &srvDesc, &shaderResourceView);

			texture->Release();

			break;


		case LightType::AMBIENT:
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.ArraySize = 6;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.MipLevels = 1;
			textureDesc.MiscFlags = 0;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			device->CreateTexture2D(&textureDesc, nullptr, &texture);

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.Texture2D.MipSlice = 0;
			device->CreateRenderTargetView(texture, &rtvDesc, &renderTargetViews[0]);

			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.TextureCube.MipLevels = -1;
			srvDesc.TextureCube.MostDetailedMip = 0;
			device->CreateShaderResourceView(texture, &srvDesc, &shaderResourceView);

			texture->Release();
			break;
	}
}

ScratchEngine::Rendering::Shadow::~Shadow()
{
	for (int i = 0; i < 6; ++i)
	{
		if (depthStencilViews[i])
			depthStencilViews[i]->Release();
		else
			break;
	}

	for (int i = 0; i < 6; ++i)
	{
		if (renderTargetViews[i])
			renderTargetViews[i]->Release();
		else
			break;
	}
}
