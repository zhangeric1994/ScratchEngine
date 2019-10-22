#include <limits>

#include "CubeMap.h"
#include "LightSource.h"
#include "Renderable.h"
#include "RenderingEngine.h"
#include "ShadowMap.h"
#include "SimpleShader.h"
#include "Texture.h"
#include "Viewer.h"

using namespace std;


constexpr XMVECTOR axes[6] = { {  1,  0,  0 },
							   { -1,  0,  0 },
							   {  0,  1,  0 },
							   {  0, -1,  0 },
							   {  0,  0,  1 },
							   {  0,  0, -1 } };

constexpr XMVECTOR axesUp[6] = { { 0, 1,  0 },
								 { 0, 1,  0 },
								 { 0, 0, -1 },
								 { 0, 0,  1 },
								 { 0, 1,  0 },
								 { 0, 1,  0 } };

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::singleton = nullptr;

ScratchEngine::Rendering::RenderingEngine::RenderingEngine(RenderingEngineConfig config) : materialAllocator(config.maxNumMeshes), meshAllocator(config.maxNumMeshes)
{
	assert(config.device);
	device = config.device;

	assert(config.deviceContext);
	deviceContext = config.deviceContext;


	shadow = nullptr;
	hasZPrepass = false;

	XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(XMVectorSet(0, 10, 0, 0), XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 0, 1, 0)));
	XMMATRIX shadowProjection = XMMatrixTranspose(XMMatrixOrthographicLH(20, 20, 0.1f, 100));

	XMStoreFloat4x4(&shadowViewProjectionMat, XMMatrixMultiply(shadowProjection, shadowView));


	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	std::string spath = std::string(buffer).substr(0, pos).c_str();
	std::wstring wpath = std::wstring(spath.begin(), spath.end());

	vsDepthOnly = new SimpleVertexShader(device, deviceContext);
	vsDepthOnly->LoadShaderFile((wpath + std::wstring(L"/VS_DepthOnly.cso")).c_str());

	vsPositionOnly = new SimpleVertexShader(device, deviceContext);
	vsPositionOnly->LoadShaderFile((wpath + std::wstring(L"/VS_PositionOnly.cso")).c_str());

	vsDirectionalLight = new SimpleVertexShader(device, deviceContext);
	vsDirectionalLight->LoadShaderFile((wpath + std::wstring(L"/VS_DirectionalLight.cso")).c_str());

	vsPointLight = new SimpleVertexShader(device, deviceContext);
	vsPointLight->LoadShaderFile((wpath + std::wstring(L"/VS_PointLight.cso")).c_str());

	vsPointLightShadow = new SimpleVertexShader(device, deviceContext);
	vsPointLightShadow->LoadShaderFile((wpath + std::wstring(L"/VS_PointLightShadow.cso")).c_str());

	vsViewport = new SimpleVertexShader(device, deviceContext);
	vsViewport->LoadShaderFile((wpath + std::wstring(L"/VS_Viewport.cso")).c_str());


	psSolidColor = new SimplePixelShader(device, deviceContext);
	psSolidColor->LoadShaderFile((wpath + std::wstring(L"/PS_SolidColor.cso")).c_str());

	psGBuffer = new SimplePixelShader(device, deviceContext);
	psGBuffer->LoadShaderFile((wpath + std::wstring(L"/PS_GBuffer.cso")).c_str());

	psDirectionalLight = new SimplePixelShader(device, deviceContext);
	psDirectionalLight->LoadShaderFile((wpath + std::wstring(L"/PS_DirectionalLightPBR.cso")).c_str());
	
	psPointLight = new SimplePixelShader(device, deviceContext);
	psPointLight->LoadShaderFile((wpath + std::wstring(L"/PS_PointLightPBR.cso")).c_str());

	psPointLightShadow = new SimplePixelShader(device, deviceContext);
	psPointLightShadow->LoadShaderFile((wpath + std::wstring(L"/PS_PointLightShadow.cso")).c_str());

	psAmbientLight = new SimplePixelShader(device, deviceContext);
	psAmbientLight->LoadShaderFile((wpath + std::wstring(L"/PS_AmbientLight.cso")).c_str());

	psDeferred = new SimplePixelShader(device, deviceContext);
	psDeferred->LoadShaderFile((wpath + std::wstring(L"/PS_CombinedDeferred.cso")).c_str());

	psShadowVolume = new SimplePixelShader(device, deviceContext);
	psShadowVolume->LoadShaderFile((wpath + std::wstring(L"/PS_ShadowVolume.cso")).c_str());


	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dsDesc, &dssLessEqual);

	dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	device->CreateDepthStencilState(&dsDesc, &dsReadGreater);

	dsDesc = {};
	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	device->CreateDepthStencilState(&dsDesc, &dsOff);

	dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	device->CreateDepthStencilState(&dsDesc, &dssCSM);


	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &bsAdditive);


	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rsDesc, &rsInsideOut);

	rsDesc = {};
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.DepthClipEnable = true;
	rsDesc.DepthBias = 1000;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&rsDesc, &rsShadow);

	rsDesc = {};
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rsDesc, &rsWireframe);
	

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSamplerDesc, &shadowSampler);


	GFSDK_SSAO_CustomHeap CustomHeap;
	CustomHeap.new_ = ::operator new;
	CustomHeap.delete_ = ::operator delete;

	assert(GFSDK_SSAO_CreateContext_D3D11(device, &ssaoContext, &CustomHeap) == GFSDK_SSAO_OK); // HBAO+ requires feature level 11_0 or above
}

ScratchEngine::Rendering::RenderingEngine::~RenderingEngine()
{
	singleton = nullptr;


	if (vsDepthOnly)
		delete vsDepthOnly;

	if (vsDirectionalLight)
		delete vsDirectionalLight;

	if (vsPointLight)
		delete vsPointLight;

	if (psGBuffer)
		delete psGBuffer;


	if (dssLessEqual)
		dssLessEqual->Release();

	if (dsReadGreater)
		dsReadGreater->Release();

	if (dsOff)
		dsOff->Release();

	if (bsAdditive)
		bsAdditive->Release();
}

void ScratchEngine::Rendering::RenderingEngine::PerformZPrepass(Viewer* viewer, Renderable* renderables, int numRenderables)
{
	deviceContext->BeginEventInt(L"Z-Prepass", 0);


	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);


	vsDepthOnly->SetMatrix4x4("viewProjection", XMMatrixMultiply(viewer->projectionMatrix, viewer->viewMatrix));
	vsDepthOnly->CopyBufferData("CameraData");
	
	vsDepthOnly->SetShader();


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	UINT indexCount = 0;

	for (int i = 0; i < numRenderables; ++i)
	{
		Renderable& renderable = renderables[i];


		vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
		vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);
		vsDepthOnly->CopyBufferData("ObjectData");


		Mesh* mesh = renderable.mesh;

		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


		indexCount += mesh->GetIndexCount();
	}


	hasZPrepass = true;


	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::DrawForward(Viewer* viewer, Renderable* renderables, int numRenderables, LightSource* lightSources, int numLightSources)
{
	if (hasZPrepass)
		deviceContext->OMSetDepthStencilState(dssLessEqual, 0);
	else
		deviceContext->OMSetDepthStencilState(nullptr, 0);


	ID3D11ShaderResourceView* shadowMap = shadow->getShadowSRV();


	XMMATRIX viewMatrix = viewer->viewMatrix;
	XMMATRIX projectionMatrix = viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
	XMVECTOR cameraPosition = viewer->position;


	i32 j = 0;

	while (j < numRenderables)
	{
		Material* material = renderables[j].material;
		SimpleVertexShader* vertexShader = material->GetVertexShader();
		SimplePixelShader* pixelShader = material->GetPixelShader();


		vertexShader->SetMatrix4x4("view", viewMatrix);
		vertexShader->SetMatrix4x4("projection", projectionMatrix);
		vertexShader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		vertexShader->CopyBufferData("CameraData");

		vertexShader->SetShader();


		pixelShader->SetInt("GammaCorrection", 1);
		pixelShader->CopyBufferData("FrameData");

		pixelShader->SetData("lights", lightSources, 128 * sizeof(LightSource));
		pixelShader->SetInt("numLights", numLightSources);
		pixelShader->CopyBufferData("LightSourceData");

		pixelShader->SetFloat3("cameraPosition", cameraPosition);
		pixelShader->CopyBufferData("CameraData");

		pixelShader->SetFloat4("tint", material->GetTint());
		pixelShader->SetInt("hasTexture", material->HasTexture());
		pixelShader->SetInt("hasNormalMap", material->HasNormalMap());
		pixelShader->SetInt("hasShadowMap", material->HasShadowMap());
		pixelShader->SetInt("hasRoughnessMap", material->HasRoughnessMap());
		pixelShader->SetInt("hasMetalnessMap", material->HasMetalnessMap());
		pixelShader->CopyBufferData("MaterialData");

		pixelShader->SetShader();

		if (material->HasTexture())
			pixelShader->SetShaderResourceView("diffuseMap", material->getTexture());

		if (material->HasNormalMap())
			pixelShader->SetShaderResourceView("normalMap", material->getNormalMap());

		if (material->HasRoughnessMap())
			pixelShader->SetShaderResourceView("roughnessMap", material->getRoughnessMap());

		if (material->HasMetalnessMap())
			pixelShader->SetShaderResourceView("metalnessMap", material->getMetalnessMap());

		pixelShader->SetSamplerState("basicSampler", material->getSampler());
		pixelShader->SetSamplerState("shadowSampler", shadowSampler);


		u32 stride = sizeof(Vertex);
		u32 offset = 0;
		u32 indexCount = 0;

		do
		{
			Renderable& renderable = renderables[j];


			vertexShader->SetMatrix4x4("world", renderable.worldMatrix);
			vertexShader->SetMatrix4x4("shadowViewProjection", shadowViewProjectionMat);
			vertexShader->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

			vertexShader->CopyBufferData("ObjectData");


			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


			indexCount += mesh->GetIndexCount();

			++j;
		} while (j < numRenderables && renderables[j].material == material);
	}
}

void ScratchEngine::Rendering::RenderingEngine::DrawGBuffers(Viewer* viewer, Renderable* renderables, int numRenderables, ID3D11RenderTargetView*const* gBuffers, int numGBuffers, ID3D11DepthStencilView* depthStencilView)
{
	deviceContext->BeginEventInt(L"Geometry Buffer", 0);


	deviceContext->OMSetRenderTargets(numGBuffers, gBuffers, depthStencilView);


	if (hasZPrepass)
		deviceContext->OMSetDepthStencilState(dssLessEqual, 0);
	else
		deviceContext->OMSetDepthStencilState(nullptr, 0);


	XMMATRIX viewMatrix = viewer->viewMatrix;
	XMMATRIX projectionMatrix = viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
	XMVECTOR cameraPosition = viewer->position;


	psGBuffer->SetShader();
	
	psGBuffer->SetInt("GammaCorrection", 1);
	psGBuffer->CopyBufferData("FrameData");

	psGBuffer->SetFloat3("cameraPosition", cameraPosition);
	psGBuffer->CopyBufferData("CameraData");


	i32 i = 0;

	while (i < numRenderables)
	{
		Material* material = renderables[i].material;
		SimpleVertexShader* vertexShader = material->GetVertexShader();


		vertexShader->SetMatrix4x4("view", viewMatrix);
		vertexShader->SetMatrix4x4("projection", projectionMatrix);
		vertexShader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		vertexShader->CopyBufferData("CameraData");

		vertexShader->SetShader();


		psGBuffer->SetFloat4("tint", material->GetTint());
		psGBuffer->SetInt("hasTexture", material->HasTexture());
		psGBuffer->SetInt("hasNormalMap", material->HasNormalMap());
		psGBuffer->SetInt("hasShadowMap", material->HasShadowMap());
		psGBuffer->SetInt("hasRoughnessMap", material->HasRoughnessMap());
		psGBuffer->SetInt("hasMetalnessMap", material->HasMetalnessMap());
		psGBuffer->CopyBufferData("MaterialData");

		if (material->HasTexture())
			psGBuffer->SetShaderResourceView("diffuseMap", material->getTexture());

		if (material->HasNormalMap())
			psGBuffer->SetShaderResourceView("normalMap", material->getNormalMap());
		
		//if (material->HasShadowMap())
		//	psGBuffer->SetShaderResourceView("ShadowMap", shadow->getShadowSRV());
		
		if (material->HasRoughnessMap())
			psGBuffer->SetShaderResourceView("roughnessMap", material->getRoughnessMap());
		
		if (material->HasMetalnessMap())
			psGBuffer->SetShaderResourceView("metalnessMap", material->getMetalnessMap());
		
		psGBuffer->SetSamplerState("basicSampler", material->getSampler());
		//	psGBuffer->SetSamplerState("shadowSampler", shadow->getSampler());


		u32 stride = sizeof(Vertex);
		u32 offset = 0;
		u32 indexCount = 0;

		do
		{
			Renderable& renderable = renderables[i];


			vertexShader->SetMatrix4x4("world", renderable.worldMatrix);
			vertexShader->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

			vertexShader->CopyBufferData("ObjectData");


			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


			indexCount += mesh->GetIndexCount();

			++i;
		} while (i < numRenderables && renderables[i].material == material);
	}


	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::DrawLightBuffer(Viewer* viewer, LightSource* lightSources, int numLightSources, ID3D11ShaderResourceView** gBuffers, ID3D11RenderTargetView* lightBuffer, ID3D11DepthStencilView* depthStencilView, ID3D11ShaderResourceView* stencilSRV)
{
	deviceContext->BeginEventInt(L"Light Buffer", 0);


	XMMATRIX viewMatrix = viewer->viewMatrix;
	XMMATRIX projectionMatrix = viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
	XMMATRIX inverseViewProjectionMatrix = XMMatrixTranspose(XMMatrixInverse(0, viewProjectionMatrix));
	XMVECTOR cameraPosition = viewer->position;


	vsDirectionalLight->SetMatrix4x4("view", viewMatrix);
	vsDirectionalLight->SetMatrix4x4("projection", projectionMatrix);
	vsDirectionalLight->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	vsDirectionalLight->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	vsDirectionalLight->SetFloat3("cameraPosition", cameraPosition);
	vsDirectionalLight->CopyBufferData("CameraData");

	vsPointLight->SetMatrix4x4("view", viewMatrix);
	vsPointLight->SetMatrix4x4("projection", projectionMatrix);
	vsPointLight->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	vsPointLight->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	vsPointLight->SetFloat3("cameraPosition", cameraPosition);
	vsPointLight->CopyBufferData("CameraData");


	psDirectionalLight->SetMatrix4x4("view", viewMatrix);
	psDirectionalLight->SetMatrix4x4("projection", projectionMatrix);
	psDirectionalLight->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	psDirectionalLight->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	psDirectionalLight->SetFloat3("cameraPosition", cameraPosition);
	psDirectionalLight->CopyBufferData("CameraData");

	psPointLight->SetMatrix4x4("view", viewMatrix);
	psPointLight->SetMatrix4x4("projection", projectionMatrix);
	psPointLight->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	psPointLight->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	psPointLight->SetFloat3("cameraPosition", cameraPosition);
	psPointLight->CopyBufferData("CameraData");


	// Set up basic render states:
	//  - Additive blending so lights add together
	//  - Depth off, so all pixels in the silhouette rasterize (this can be optimized!!!)
	deviceContext->OMSetBlendState(bsAdditive, 0, 0xFFFFFFFF);


	i32 i = 0;

	while (i < numLightSources)
	{
		LightType lightType = lightSources[i].type;


		u32 stride = sizeof(Vertex);
		u32 offset = 0;
		u32 indexCount = 0;

		switch (lightType)
		{
		case LightType::DIRECTIONAL:
			deviceContext->OMSetRenderTargets(1, &lightBuffer, nullptr);
			deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
			deviceContext->RSSetState(0);

			do
			{
				LightSource lightSource = lightSources[i];


				vsDirectionalLight->SetShader();


				psDirectionalLight->SetData("light", &lightSource, sizeof(LightSource));
				psDirectionalLight->CopyBufferData("LightData");

				psDirectionalLight->SetShader();

				psDirectionalLight->SetShaderResourceView("gBufferAlbedo", gBuffers[0]);
				psDirectionalLight->SetShaderResourceView("gBufferNormal", gBuffers[1]);
				psDirectionalLight->SetShaderResourceView("gBufferDepth", gBuffers[2]);
				psDirectionalLight->SetShaderResourceView("gBufferMaterial", gBuffers[3]);
				psDirectionalLight->SetShaderResourceView("stencilBuffer", stencilSRV);

				if (lightSource.shadow)
					psDirectionalLight->SetShaderResourceView("shadowMap", lightSource.shadow->shaderResourceView);

				psDirectionalLight->SetSamplerState("shadowSampler", shadowSampler);


				deviceContext->Draw(3, 0);


				++i;
			} while (i < numLightSources && lightSources[i].type == lightType);

			break;


		case LightType::POINT:
			deviceContext->OMSetRenderTargets(1, &lightBuffer, depthStencilView);
			deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
			deviceContext->RSSetState(rsInsideOut);

			do
			{
				LightSource lightSource = lightSources[i];


				XMMATRIX T = XMMatrixTranslation(lightSource.position.x, lightSource.position.y, lightSource.position.z);

				float s = lightSource.range * 2;
				XMMATRIX S = XMMatrixScaling(s, s, s); // This sphere model has a radius of 0.5, so double the scale
					
				vsPointLight->SetMatrix4x4("world", XMMatrixTranspose(S * T));
				vsPointLight->CopyBufferData("ObjectData");

				vsPointLight->SetShader();


				psPointLight->SetData("light", &lightSource, sizeof(LightSource));
				psPointLight->CopyBufferData("LightData");

				psPointLight->SetShader();

				psPointLight->SetShaderResourceView("gBufferAlbedo", gBuffers[0]);
				psPointLight->SetShaderResourceView("gBufferNormal", gBuffers[1]);
				psPointLight->SetShaderResourceView("gBufferDepth", gBuffers[2]);
				psPointLight->SetShaderResourceView("gBufferMaterial", gBuffers[3]);

				if (lightSource.shadow)
					psPointLight->SetShaderResourceView("shadowMap", lightSource.shadow->shaderResourceView);

				psPointLight->SetSamplerState("shadowSampler", shadowSampler);

					
				ID3D11Buffer* vertexBuffer = sphereMesh->GetVertexBuffer();
				ID3D11Buffer* indexBuffer = sphereMesh->GetIndexBuffer();


				deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
				deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

				// Finally do the actual drawing
				deviceContext->DrawIndexed(sphereMesh->GetIndexCount(), 0, 0);


				indexCount += sphereMesh->GetIndexCount();

				++i;
			} while (i < numLightSources && lightSources[i].type == lightType);

			break;


		case LightType::AMBIENT:
			deviceContext->OMSetRenderTargets(1, &lightBuffer, nullptr);
			deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
			deviceContext->RSSetState(0);

			do
			{
				LightSource lightSource = lightSources[i];


				vsDirectionalLight->SetShader();


				psAmbientLight->SetData("light", &lightSource, sizeof(LightSource));
				psAmbientLight->CopyBufferData("LightData");

				psAmbientLight->SetShader();

				psAmbientLight->SetShaderResourceView("gBufferAlbedo", gBuffers[0]);

				if (lightSource.shadow)
					psAmbientLight->SetShaderResourceView("ssaoBuffer", lightSource.shadow->shaderResourceView);

				deviceContext->Draw(3, 0);


				indexCount += sphereMesh->GetIndexCount();

				++i;
			} while (i < numLightSources && lightSources[i].type == lightType);

			break;
		}
	}


	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::DrawDeferred(ID3D11ShaderResourceView* lightBuffer, ID3D11RenderTargetView* backBuffer, ID3D11DepthStencilView* depthStencilView)
{
	deviceContext->BeginEventInt(L"Deferred", 0);


	// Reset for final combine rendering
	deviceContext->OMSetDepthStencilState(dsOff, 0);
	deviceContext->OMSetRenderTargets(1, &backBuffer, depthStencilView);
	deviceContext->OMSetBlendState(0, 0, 0xFFFFFFFF);
	deviceContext->RSSetState(0);


	// Using PBR, which already has taken albedo into account by this point
	// since the PBR light functions are balancing albedo/specular.  This means
	// our "light buffer" already has the final lighting for the scene.
	vsViewport->SetShader();

	
	psDeferred->SetInt("GammaCorrection", 1);
	psDeferred->CopyAllBufferData();

	psDeferred->SetShader();

	psDeferred->SetShaderResourceView("lightBuffer", lightBuffer);
	

	deviceContext->Draw(3, 0);


	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::RenderSSAO(LightSource* light, ID3D11ShaderResourceView* depthBuffer, ID3D11ShaderResourceView* normalBuffer)
{
	if (light->type != LightType::AMBIENT || !light->shadow)
		return;


	RenderSSAO(light->shadow->renderTargetViews[0], &light->shadowProjection, depthBuffer, normalBuffer);
}

void ScratchEngine::Rendering::RenderingEngine::RenderSSAO(ID3D11RenderTargetView* ssaoBuffer, XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* depthBuffer, ID3D11ShaderResourceView* normalBuffer)
{
	GFSDK_SSAO_Float4x4 P = {};

	memcpy(&P.Array, projectionMatrix, sizeof(P.Array));


	GFSDK_SSAO_InputData_D3D11 ssaoInput;
	ssaoInput.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	ssaoInput.DepthData.pFullResDepthTextureSRV = depthBuffer;
	ssaoInput.DepthData.ProjectionMatrix.Data = P;
	ssaoInput.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_COLUMN_MAJOR_ORDER;
	ssaoInput.DepthData.MetersToViewSpaceUnits = 1;
	ssaoInput.NormalData.pFullResNormalTextureSRV = normalBuffer;
	ssaoInput.NormalData.Enable = false;


	GFSDK_SSAO_Parameters_D3D11 ssaoParams;
	ssaoParams.Radius = 2.0f;
	ssaoParams.Bias = 0.1f;
	ssaoParams.PowerExponent = 2.0f;
	ssaoParams.Blur.Enable = true;
	ssaoParams.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_8;
	ssaoParams.Blur.Sharpness = 2.0f;
	ssaoParams.Output.BlendMode = GFSDK_SSAO_OVERWRITE_RGB;

	GFSDK_SSAO_Status status = ssaoContext->RenderAO(deviceContext, &ssaoInput, &ssaoParams, ssaoBuffer);
}

void ScratchEngine::Rendering::RenderingEngine::RenderShadowMap(LightSource* light, Renderable* renderables, int numRenderables)
{
	if (light->type == LightType::AMBIENT)
		return;


	Shadow* shadow = light->shadow;

	if (shadow)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.Width = shadow->width;
		viewport.Height = shadow->height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;


		switch (light->type)
		{
		case LightType::DIRECTIONAL:
			deviceContext->ClearDepthStencilView(shadow->depthStencilViews[0], D3D11_CLEAR_DEPTH, 1.0f, 0);

			deviceContext->RSSetState(rsShadow);
			deviceContext->RSSetViewports(1, &viewport);
			deviceContext->PSSetShader(nullptr, nullptr, 0);
			deviceContext->OMSetRenderTargets(0, nullptr, shadow->depthStencilViews[0]);


			vsDepthOnly->SetMatrix4x4("viewProjection", light->shadowViewProjection[0]);

			vsDepthOnly->CopyBufferData("CameraData");

			vsDepthOnly->SetShader();


			for (int i = 0; i < numRenderables; ++i)
			{
				Renderable& renderable = renderables[i];


				vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
				vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

				vsDepthOnly->CopyBufferData("ObjectData");


				Mesh* mesh = renderable.mesh;

				ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
				ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


				UINT stride = sizeof(Vertex);
				UINT offset = 0;
				UINT indexCount = 0;

				deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
				deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


				indexCount += mesh->GetIndexCount();
			}

			break;


		case LightType::POINT:
			deviceContext->RSSetState(rsShadow);
			deviceContext->RSSetViewports(1, &viewport);


			vsPointLightShadow->SetData("light", light, sizeof(LightSource));
			vsPointLightShadow->CopyBufferData("LightData");

			vsPointLightShadow->SetShader();


			psPointLightShadow->SetData("light", light, sizeof(LightSource));

			psPointLightShadow->CopyBufferData("LightData");

			psPointLightShadow->SetShader();


			XMVECTOR lightPosition = XMLoadFloat3(&light->position);
			XMMATRIX P = XMMatrixTranspose(XMMatrixPerspectiveFovLH(g_XMHalfPi.f[0], 1, 0.01f, light->range));

			for (int i = 0; i < 6; ++i)
			{
				deviceContext->ClearDepthStencilView(shadow->depthStencilViews[i], D3D11_CLEAR_DEPTH, 1.0f, 0);
				deviceContext->OMSetRenderTargets(0, nullptr, shadow->depthStencilViews[i]);


				vsPointLightShadow->SetMatrix4x4("viewProjection", XMMatrixMultiply(P, XMMatrixTranspose(XMMatrixLookToLH(lightPosition, axes[i], axesUp[i]))));
				
				vsPointLightShadow->CopyBufferData("CameraData");


				for (int j = 0; j < numRenderables; ++j)
				{
					Renderable& renderable = renderables[j];


					vsPointLightShadow->SetMatrix4x4("world", renderable.worldMatrix);
					vsPointLightShadow->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

					vsPointLightShadow->CopyBufferData("ObjectData");


					Mesh* mesh = renderable.mesh;

					ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
					ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


					UINT stride = sizeof(Vertex);
					UINT offset = 0;
					UINT indexCount = 0;

					deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
					deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
					deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


					indexCount += mesh->GetIndexCount();
				}
			}

			break;
		}
	}
}

void ScratchEngine::Rendering::RenderingEngine::RenderCSM(const CSMConfig& config, Renderable* renderables, int numRenderables)
{
	LightSource* light = config.light;
	Shadow* shadow = light->shadow;

	int N = config.numCascades;
	//float nz = config.nearZ;
	//float fz = config.farZ;
	float f = config.selectionFactor;
	float q = config.powerExponent;

	//float dz = fz - nz;
	//

	//float hfov = config.fov / 2
	//float tanx = 

	//float zn = nz;
	//float dzu = dz / N;
	//float dzl = dz / (pow(2, N) - 1);

	//for (int i = 0; i < N; ++i)
	//{
	//	float zf = i == N - 1 ? fz : (nz + (dzu + (dzu - dzl) * t));

	//	float xn = zn * tanx;
	//	float xf;
	//	float yn;
	//	float yf;


	//	zn = zf;
	//	dzl *= 2;
	//}

	//Z.push_back(fz);


	//for (int i = 0; i < N; ++i)


	XMMATRIX viewMatrix = config.viewer->viewMatrix;
	XMMATRIX projectionMatrix = config.viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix));
	XMMATRIX inverseViewProjectionMatrix = XMMatrixInverse(nullptr, viewProjectionMatrix);

	XMVECTOR lightDirection = XMLoadFloat3(&light->direction);
	XMMATRIX lightViewMatrix = XMMatrixLookToLH(XMVectorZero(), lightDirection, { 0, 1, 0 });
	XMMATRIX lightInverseViewMatrix = XMMatrixInverse(nullptr, lightViewMatrix);

	XMMATRIX R = XMMatrixRotationAxis(XMVector3Cross({ 0, 0, 1 }, lightDirection), XMVector3AngleBetweenNormals({ 0, 0, 1 }, lightDirection).m128_f32[0]);


	D3D11_VIEWPORT shadowViewport = {};
	shadowViewport.Width = shadow->width;
	shadowViewport.Height = shadow->height;
	shadowViewport.MinDepth = 0.f;
	shadowViewport.MaxDepth = 1.f;
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;


	XMVECTOR V;

	V = XMVector4Transform({ -1, -1, -1, 1 }, inverseViewProjectionMatrix);
	V = XMVectorScale(V, 1 / V.m128_f32[3]);
	XMVECTOR lbn = V;
	XMVECTOR leftBottomNear = XMVector3TransformCoord(V, lightViewMatrix);
	
	V = XMVector4Transform({ 1, -1, -1, 1 }, inverseViewProjectionMatrix);
	V = XMVectorScale(V, 1 / V.m128_f32[3]);
	XMVECTOR rightBottomNear = XMVector3TransformCoord(V, lightViewMatrix);

	V = XMVector4Transform({ -1, 1, -1, 1 }, inverseViewProjectionMatrix);
	V = XMVectorScale(V, 1 / V.m128_f32[3]);
	XMVECTOR leftUpNear = XMVector3TransformCoord(V, lightViewMatrix);

	V = XMVector4Transform({ 1, 1, -1, 1 }, inverseViewProjectionMatrix);
	V = XMVectorScale(V, 1 / V.m128_f32[3]);
	XMVECTOR rightUpNear = XMVector3TransformCoord(V, lightViewMatrix);

	V = XMVector4Transform({ -1, -1, 1, 1 }, inverseViewProjectionMatrix);
	V = XMVectorScale(V, 1 / V.m128_f32[3]);
	XMVECTOR lbf = V;
	XMVECTOR leftBottomFar = XMVector3TransformCoord(V, lightViewMatrix);

	V = XMVector4Transform({ 1, -1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightBottomFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ -1, 1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR leftUpFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ 1, 1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightUpFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);


	deviceContext->BeginEventInt(L"CSM", 0);


	deviceContext->PSSetShader(nullptr, nullptr, 0);


	float dz = 1 / (float)N;
	float p = (1 - pow(q, N)) / (1 - q);

	XMVECTOR leftBottom = leftBottomFar;
	XMVECTOR rightBottom = rightBottomFar;
	XMVECTOR leftUp = leftUpFar;
	XMVECTOR rightUp = rightUpFar;

	for (int i = N - 1; i >= 0; --i)
	{
		deviceContext->BeginEventInt(L"Cascade#d", i);

		XMVECTOR min = { numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max() };
		XMVECTOR max = { numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest() };

		min = XMVectorMin(min, leftBottom);
		min = XMVectorMin(min, rightBottom);
		min = XMVectorMin(min, leftUp);
		min = XMVectorMin(min, rightUp);
		max = XMVectorMax(max, leftBottom);
		max = XMVectorMax(max, rightBottom);
		max = XMVectorMax(max, leftUp);
		max = XMVectorMax(max, rightUp);


		float t = i * dz;
		t = i == 0 ? 0 : (1 - f) * t + f * (1 - pow(q, i)) / (1 - q) / p;

		leftBottom = XMVectorLerp(leftBottomNear, leftBottomFar, t);
		rightBottom = XMVectorLerp(rightBottomNear, rightBottomFar, t);
		leftUp = XMVectorLerp(leftUpNear, leftUpFar, t);
		rightUp = XMVectorLerp(rightUpNear, rightUpFar, t);


		XMVECTOR VVVV = XMVector3TransformCoord(leftBottom, lightInverseViewMatrix);
		XMVECTOR AAAA = XMVectorLerp(lbn, lbf, t);


		min = XMVectorMin(min, leftBottom);
		min = XMVectorMin(min, rightBottom);
		min = XMVectorMin(min, leftUp);
		min = XMVectorMin(min, rightUp);
		max = XMVectorMax(max, leftBottom);
		max = XMVectorMax(max, rightBottom);
		max = XMVectorMax(max, leftUp);
		max = XMVectorMax(max, rightUp);


		XMVECTOR size = XMVectorSubtract(max, min);
		XMVECTOR center = XMVectorScale(XMVectorAdd(min, max), 0.5f);

		float extraDepth = __max(10.0f, size.m128_f32[2]);

		XMVECTOR shadowTranslation = -center;
		shadowTranslation.m128_f32[2] = extraDepth - min.m128_f32[2];

		XMMATRIX shadowViewProjectionMatrix = XMMatrixTranspose(lightViewMatrix * XMMatrixTranslationFromVector(shadowTranslation) * XMMatrixOrthographicLH(size.m128_f32[0], size.m128_f32[1], 0, 2 * extraDepth + size.m128_f32[2]));

		XMMATRIX T = XMMatrixTranslationFromVector(XMVector3TransformCoord(center, lightInverseViewMatrix));
		XMMATRIX S = XMMatrixScaling(size.m128_f32[0], size.m128_f32[1], 2 * extraDepth + size.m128_f32[2]);
		

		light->shadowViewProjection[i] = shadowViewProjectionMatrix;


		deviceContext->ClearDepthStencilView(shadow->depthStencilViews[i], D3D11_CLEAR_DEPTH, 1.0f, 0);
		
		deviceContext->RSSetState(rsShadow);
		deviceContext->RSSetViewports(1, &shadowViewport);
		deviceContext->OMSetRenderTargets(0, nullptr, shadow->depthStencilViews[i]);
		deviceContext->OMSetDepthStencilState(nullptr, 0);


		vsDepthOnly->SetShader();

		vsDepthOnly->SetMatrix4x4("viewProjection", shadowViewProjectionMatrix);

		vsDepthOnly->CopyBufferData("CameraData");


		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		UINT indexCount = 0;

		for (int j = 0; j < numRenderables; ++j)
		{
			Renderable& renderable = renderables[j];


			vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
			vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

			vsDepthOnly->CopyBufferData("ObjectData");


			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


			indexCount += mesh->GetIndexCount();
		}


		deviceContext->SetMarkerInt(L"Shadow Volume", 0);


		deviceContext->RSSetState(rsInsideOut);
		deviceContext->RSSetViewports(1, config.viewport);
		deviceContext->OMSetRenderTargets(0, nullptr, config.depthStencilView);
		deviceContext->OMSetDepthStencilState(dssCSM, i);


		vsPositionOnly->SetMatrix4x4("view", viewMatrix);
		vsPositionOnly->SetMatrix4x4("projection", projectionMatrix);
		vsPositionOnly->SetMatrix4x4("world", XMMatrixTranspose(S * R * T));
		vsPositionOnly->CopyAllBufferData();

		vsPositionOnly->SetShader();


		ID3D11Buffer* vertexBuffer = cubeMesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = cubeMesh->GetIndexBuffer();


		offset = 0;
		indexCount = 0;


		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);


		indexCount += cubeMesh->GetIndexCount();


		deviceContext->EndEvent();
	}

	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::RenderCSMDebug(const CSMConfig& config, Renderable* renderables, int numRenderables, ID3D11RenderTargetView* debugView)
{
	XMVECTOR colors[6] =
	{
		{ 1, 0, 0 },
		{ 1, 1, 0 },
		{ 0, 1, 0 },
		{ 0, 1, 1 },
		{ 0, 0, 1 },
		{ 1, 0, 1 },
	};


	LightSource* light = config.light;
	Shadow* shadow = light->shadow;

	int N = config.numCascades;
	//float nz = config.nearZ;
	//float fz = config.farZ;
	float f = config.selectionFactor;
	float q = config.powerExponent;

	//float dz = fz - nz;
	//

	//float hfov = config.fov / 2
	//float tanx = 

	//float zn = nz;
	//float dzu = dz / N;
	//float dzl = dz / (pow(2, N) - 1);

	//for (int i = 0; i < N; ++i)
	//{
	//	float zf = i == N - 1 ? fz : (nz + (dzu + (dzu - dzl) * t));

	//	float xn = zn * tanx;
	//	float xf;
	//	float yn;
	//	float yf;


	//	zn = zf;
	//	dzl *= 2;
	//}

	//Z.push_back(fz);


	//for (int i = 0; i < N; ++i)


	XMMATRIX viewMatrix = config.viewer->viewMatrix;
	XMMATRIX projectionMatrix = config.viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(XMMatrixTranspose(viewMatrix), XMMatrixTranspose(projectionMatrix));
	XMMATRIX inverseViewProjectionMatrix = XMMatrixInverse(nullptr, viewProjectionMatrix);

	XMVECTOR lightDirection = XMLoadFloat3(&light->direction);
	XMMATRIX lightViewMatrix = XMMatrixLookToLH(XMVectorZero(), lightDirection, { 0, 1, 0 });
	XMMATRIX lightInverseViewMatrix = XMMatrixInverse(nullptr, lightViewMatrix);


	D3D11_VIEWPORT shadowViewport = {};
	shadowViewport.Width = shadow->width;
	shadowViewport.Height = shadow->height;
	shadowViewport.MinDepth = 0.f;
	shadowViewport.MaxDepth = 1.f;
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;


	XMVECTOR V;

	V = XMVector4Transform({ -1, -1, -1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR leftBottomNear = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ 1, -1, -1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightBottomNear = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ -1, 1, -1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR leftUpNear = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ 1, 1, -1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightUpNear = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ -1, -1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR leftBottomFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ 1, -1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightBottomFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ -1, 1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR leftUpFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);

	V = XMVector4Transform({ 1, 1, 1, 1 }, inverseViewProjectionMatrix);
	XMVECTOR rightUpFar = XMVector3TransformCoord(XMVectorScale(V, 1 / V.m128_f32[3]), lightViewMatrix);


	deviceContext->BeginEventInt(L"CSM", 0);


	deviceContext->PSSetShader(nullptr, nullptr, 0);


	float dz = 1 / (float)N;
	float p = (1 - pow(q, N)) / (1 - q);

	XMVECTOR leftBottom = leftBottomFar;
	XMVECTOR rightBottom = rightBottomFar;
	XMVECTOR leftUp = leftUpFar;
	XMVECTOR rightUp = rightUpFar;

	for (int i = N - 1; i >= 0; --i)
	{
		deviceContext->BeginEventInt(L"Cascade#d", i);

		XMVECTOR min = { numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max() };
		XMVECTOR max = { numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest(), numeric_limits<float>::lowest() };

		min = XMVectorMin(min, leftBottom);
		min = XMVectorMin(min, rightBottom);
		min = XMVectorMin(min, leftUp);
		min = XMVectorMin(min, rightUp);
		max = XMVectorMax(max, leftBottom);
		max = XMVectorMax(max, rightBottom);
		max = XMVectorMax(max, leftUp);
		max = XMVectorMax(max, rightUp);


		float t = i * dz;
		t = i == 0 ? 0 : (1 - f) * t + f * (1 - pow(q, i)) / (1 - q) / p;

		leftBottom = XMVectorLerp(leftBottomNear, leftBottomFar, t);
		rightBottom = XMVectorLerp(rightBottomNear, rightBottomFar, t);
		leftUp = XMVectorLerp(leftUpNear, leftUpFar, t);
		rightUp = XMVectorLerp(rightUpNear, rightUpFar, t);


		min = XMVectorMin(min, leftBottom);
		min = XMVectorMin(min, rightBottom);
		min = XMVectorMin(min, leftUp);
		min = XMVectorMin(min, rightUp);
		max = XMVectorMax(max, leftBottom);
		max = XMVectorMax(max, rightBottom);
		max = XMVectorMax(max, leftUp);
		max = XMVectorMax(max, rightUp);


		XMVECTOR D = XMVectorSubtract(max, min);
		XMVECTOR center = XMVectorScale(XMVectorAdd(min, max), 0.5f);

		float extraDepth = __max(10.0f, D.m128_f32[2]);

		XMVECTOR shadowTranslation = -center;
		shadowTranslation.m128_f32[2] = extraDepth - min.m128_f32[2];

		XMMATRIX shadowViewProjectionMatrix = XMMatrixTranspose(lightViewMatrix * XMMatrixTranslationFromVector(shadowTranslation) * XMMatrixOrthographicLH(D.m128_f32[0], D.m128_f32[1], 0, 2 * extraDepth + D.m128_f32[2]));

		XMMATRIX T = XMMatrixTranslationFromVector(XMVector3TransformCoord(center, lightInverseViewMatrix));
		XMMATRIX R = XMMatrixRotationAxis(XMVector3Cross(lightDirection, { 0, 0, 1 }), -XMVector3AngleBetweenVectors({ 0, 0, 1 }, lightDirection).m128_f32[0]);
		XMMATRIX S = XMMatrixScaling(D.m128_f32[0], D.m128_f32[1], 2 * extraDepth + D.m128_f32[2]);


		light->shadowViewProjection[i] = shadowViewProjectionMatrix;


		deviceContext->ClearDepthStencilView(shadow->depthStencilViews[i], D3D11_CLEAR_DEPTH, 1.0f, 0);

		deviceContext->RSSetState(rsShadow);
		deviceContext->RSSetViewports(1, &shadowViewport);
		deviceContext->OMSetRenderTargets(0, nullptr, shadow->depthStencilViews[i]);
		deviceContext->OMSetDepthStencilState(nullptr, 0);


		vsDepthOnly->SetShader();

		vsDepthOnly->SetMatrix4x4("viewProjection", shadowViewProjectionMatrix);

		vsDepthOnly->CopyBufferData("CameraData");


		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		UINT indexCount = 0;

		for (int j = 0; j < numRenderables; ++j)
		{
			Renderable& renderable = renderables[j];


			vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
			vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

			vsDepthOnly->CopyBufferData("ObjectData");


			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();


			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);


			indexCount += mesh->GetIndexCount();
		}


		deviceContext->SetMarkerInt(L"Shadow Volume", 0);


		deviceContext->RSSetState(rsInsideOut);
		deviceContext->RSSetViewports(1, config.viewport);
		deviceContext->OMSetRenderTargets(0, nullptr, config.depthStencilView);
		deviceContext->OMSetDepthStencilState(dssCSM, i);


		vsPositionOnly->SetMatrix4x4("view", viewMatrix);
		vsPositionOnly->SetMatrix4x4("projection", projectionMatrix);
		vsPositionOnly->SetMatrix4x4("world", XMMatrixTranspose(S * R * T));
		vsPositionOnly->CopyAllBufferData();

		vsPositionOnly->SetShader();


		ID3D11Buffer* vertexBuffer = cubeMesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = cubeMesh->GetIndexBuffer();


		offset = 0;
		indexCount = 0;


		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);


		indexCount += cubeMesh->GetIndexCount();


		deviceContext->SetMarkerInt(L"Debug Shadow Volume", 0);


		deviceContext->RSSetState(rsWireframe);
		deviceContext->OMSetRenderTargets(1, &debugView, config.depthStencilView);
		deviceContext->OMSetDepthStencilState(dssCSM, i);


		psSolidColor->SetFloat4("tint", colors[i]);
		psSolidColor->CopyAllBufferData();

		psSolidColor->SetShader();


		offset = 0;
		indexCount = 0;


		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		deviceContext->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);


		indexCount += cubeMesh->GetIndexCount();
		

		deviceContext->EndEvent();
	}

	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::SetShadowMap(ShadowMap * _shadow)
{
	shadow = _shadow;
}

void ScratchEngine::Rendering::RenderingEngine::RenderCubeMap(CubeMap* cubeMap, Viewer* viewer)
{
	deviceContext->BeginEventInt(L"CubeMap", 1);


	ID3D11Buffer* cubeVB = cubeMap->getVB();
	ID3D11Buffer* cubeIB = cubeMap->getIB();

	u32 stride = sizeof(Vertex);
	u32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &cubeVB, &stride, &offset);
	deviceContext->IASetIndexBuffer(cubeIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX viewMatrix = viewer->viewMatrix;
	XMMATRIX projectionMatrix = viewer->projectionMatrix;

	SimpleVertexShader* cubeVS = cubeMap->getVS();
	SimplePixelShader* cubePS = cubeMap->getPS();

	cubeVS->SetMatrix4x4("view", viewMatrix);
	cubeVS->SetMatrix4x4("projection", projectionMatrix);
	cubeVS->CopyAllBufferData();
	cubeVS->SetShader();

	cubePS->SetShaderResourceView("cubeTexture", cubeMap->getSRV());
	cubePS->SetSamplerState("basicSampler", cubeMap->getSampler());
	cubePS->SetShader();

	deviceContext->RSSetState(cubeMap->getRS());
	deviceContext->OMSetDepthStencilState(cubeMap->getDSS(), 0);

	deviceContext->DrawIndexed(cubeMap->getIndexCount(), 0, 0);


	deviceContext->EndEvent();
}

void ScratchEngine::Rendering::RenderingEngine::DrawCSMIndices(Viewer* viewer, LightSource* lightSources, int numLightSources, ID3D11ShaderResourceView** gBuffers, ID3D11RenderTargetView* lightBuffer, ID3D11DepthStencilView* depthStencilView, ID3D11ShaderResourceView* stencilSRV)
{
	XMVECTOR cameraPosition = viewer->position;
	XMMATRIX viewMatrix = viewer->viewMatrix;
	XMMATRIX projectionMatrix = viewer->projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);

	XMMATRIX inverseViewProjectionMatrix = XMMatrixInverse(0, XMMatrixTranspose(viewProjectionMatrix));
	

	vsDirectionalLight->SetMatrix4x4("view", viewMatrix);
	vsDirectionalLight->SetMatrix4x4("projection", projectionMatrix);
	vsDirectionalLight->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	vsDirectionalLight->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	vsDirectionalLight->SetFloat3("cameraPosition", cameraPosition);
	vsDirectionalLight->CopyBufferData("CameraData");

	psShadowVolume->SetMatrix4x4("view", viewMatrix);
	psShadowVolume->SetMatrix4x4("projection", projectionMatrix);
	psShadowVolume->SetMatrix4x4("viewProjection", viewProjectionMatrix);
	psShadowVolume->SetMatrix4x4("inverseViewProjection", inverseViewProjectionMatrix);
	psShadowVolume->SetFloat3("cameraPosition", cameraPosition);
	psShadowVolume->CopyBufferData("CameraData");


	deviceContext->OMSetRenderTargets(1, &lightBuffer, nullptr);
	deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
	deviceContext->RSSetState(0);


	// Set up basic render states:
	//  - Additive blending so lights add together
	//  - Depth off, so all pixels in the silhouette rasterize (this can be optimized!!!)
	deviceContext->OMSetBlendState(bsAdditive, 0, 0xFFFFFFFF);


	for (int i = 0; i < numLightSources; ++i)
	{
		LightType lightType = lightSources[i].type;


		u32 stride = sizeof(Vertex);
		u32 offset = 0;
		u32 indexCount = 0;

		if (lightType == LightType::DIRECTIONAL)
		{
			LightSource lightSource = lightSources[i];


			vsDirectionalLight->SetShader();


			psShadowVolume->SetData("light", &lightSource, sizeof(LightSource));
			psShadowVolume->CopyBufferData("LightData");

			psShadowVolume->SetShader();

			psShadowVolume->SetShaderResourceView("gBufferAlbedo", gBuffers[0]);
			psShadowVolume->SetShaderResourceView("gBufferNormal", gBuffers[1]);
			psShadowVolume->SetShaderResourceView("gBufferDepth", gBuffers[2]);
			psShadowVolume->SetShaderResourceView("gBufferMaterial", gBuffers[3]);
			psShadowVolume->SetShaderResourceView("depthStencil", stencilSRV);

			if (lightSource.shadow)
				psShadowVolume->SetShaderResourceView("shadowMap", lightSource.shadow->shaderResourceView);

			psShadowVolume->SetSamplerState("shadowSampler", shadowSampler);


			deviceContext->Draw(3, 0);


			return;
		}
	}
}
