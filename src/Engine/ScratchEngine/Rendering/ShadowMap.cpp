#include "ShadowMap.h"

ScratchEngine::Rendering::ShadowMap::ShadowMap() {
	//texture2d desc
	shadowDesc = {};
	shadowDesc.Width = 1024;
	shadowDesc.Height = 1024;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;

	//dsv desc
	depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//srv desc
	shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	//render state desc
	shadowRenderStateDesc = {};
	shadowRenderStateDesc.CullMode = D3D11_CULL_BACK;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;
	shadowRenderStateDesc.DepthBias = 1000;
	shadowRenderStateDesc.DepthBiasClamp = 0.0f;
	shadowRenderStateDesc.SlopeScaledDepthBias = 1.0f;

	//sampler desc
	shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;

	shadowTexture = 0;
	shadowSampler = 0;
	shadowDepthStencilView = 0;
	shadowResourceView = 0;
	shadowRasterizerState = 0;
}

ScratchEngine::Rendering::ShadowMap::~ShadowMap() {
	//if (shadowTexture) shadowTexture->Release();
	if (shadowSampler) shadowSampler->Release();
	if (shadowDepthStencilView) shadowDepthStencilView->Release();
	if (shadowResourceView) shadowResourceView->Release();
	if (shadowRasterizerState) shadowRasterizerState->Release();
	// if (shadowShader) delete shadowShader;
}

void ScratchEngine::Rendering::ShadowMap::setUp(ID3D11Device* device) {
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);
	device->CreateDepthStencilView(shadowTexture, &depthStencilViewDesc, &shadowDepthStencilView);
	device->CreateShaderResourceView(shadowTexture, &shaderResourceViewDesc, &shadowResourceView);
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);
	device->CreateRasterizerState(&shadowRenderStateDesc, &shadowRasterizerState);

	shadowTexture->Release();
}

//void ScratchEngine::Rendering::ShadowMap::setViewport(float height, float width) {
//	shadowViewport.Height = height;
//	shadowViewport.Width = width;
//}

ID3D11DepthStencilView * ScratchEngine::Rendering::ShadowMap::getShadowDSV() {
	return shadowDepthStencilView;
}

//D3D11_VIEWPORT ScratchEngine::Rendering::ShadowMap::getViewport() {
//	return shadowViewport;
//}

ID3D11RasterizerState * ScratchEngine::Rendering::ShadowMap::getRasterizerState() {
	return shadowRasterizerState;
}

ID3D11ShaderResourceView * ScratchEngine::Rendering::ShadowMap::getShadowSRV() {
	return shadowResourceView;
}

ID3D11SamplerState * ScratchEngine::Rendering::ShadowMap::getSampler() {
	return shadowSampler;
}


