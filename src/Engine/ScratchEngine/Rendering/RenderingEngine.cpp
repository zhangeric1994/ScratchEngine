#include "CubeMap.h"
#include "LightSource.h"
#include "Renderable.h"
#include "RenderingEngine.h"
#include "ShadowMap.h"
#include "SimpleShader.h"
#include "Texture.h"
#include "Viewer.h"


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

	vsDirectionalLight = new SimpleVertexShader(device, deviceContext);
	vsDirectionalLight->LoadShaderFile((wpath + std::wstring(L"/VS_DirectionalLight.cso")).c_str());

	vsPointLight = new SimpleVertexShader(device, deviceContext);
	vsPointLight->LoadShaderFile((wpath + std::wstring(L"/VS_PointLight.cso")).c_str());

	vsViewport = new SimpleVertexShader(device, deviceContext);
	vsViewport->LoadShaderFile((wpath + std::wstring(L"/VS_Viewport.cso")).c_str());

	psGBuffer = new SimplePixelShader(device, deviceContext);
	psGBuffer->LoadShaderFile((wpath + std::wstring(L"/PS_GBuffer.cso")).c_str());

	psDirectionalLight = new SimplePixelShader(device, deviceContext);
	psDirectionalLight->LoadShaderFile((wpath + std::wstring(L"/PS_DirectionalLightPBR.cso")).c_str());
	
	psPointLight = new SimplePixelShader(device, deviceContext);
	psPointLight->LoadShaderFile((wpath + std::wstring(L"/PS_PointLightPBR.cso")).c_str());

	psDeferred = new SimplePixelShader(device, deviceContext);
	psDeferred->LoadShaderFile((wpath + std::wstring(L"/PS_CombinedDeferred.cso")).c_str());
	

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
	rsDesc.DepthClipEnable = true;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	device->CreateRasterizerState(&rsDesc, &rsInsideOut);

	rsDesc = {};
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.DepthClipEnable = true;
	rsDesc.DepthBias = 1000;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&rsDesc, &rsShadow);


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
		vertexShader->SetMatrix4x4("shadowViewProjection", shadowViewProjectionMat);
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
}

void ScratchEngine::Rendering::RenderingEngine::DrawLightBuffer(Viewer* viewer, LightSource* lightSources, int numLightSources, ID3D11ShaderResourceView** gBuffers, ID3D11RenderTargetView* lightBuffer, ID3D11DepthStencilView* depthStencilView)
{
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


	deviceContext->OMSetRenderTargets(1, &lightBuffer, depthStencilView);

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

					if (lightSource.shadow)
						psDirectionalLight->SetShaderResourceView("shadowMap", lightSource.shadow->shaderResourceView);

					psDirectionalLight->SetSamplerState("shadowSampler", shadowSampler);


					deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
					deviceContext->RSSetState(0);

					deviceContext->Draw(3, 0);


					++i;
				} while (i < numLightSources && lightSources[i].type == lightType);
				break;


			case LightType::POINT:
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

					
					ID3D11Buffer* vertexBuffer = sphereMesh->GetVertexBuffer();
					ID3D11Buffer* indexBuffer = sphereMesh->GetIndexBuffer();


					deviceContext->OMSetDepthStencilState(dsReadGreater, 0);
					deviceContext->RSSetState(rsInsideOut);

					deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
					deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

					// Finally do the actual drawing
					deviceContext->DrawIndexed(sphereMesh->GetIndexCount(), 0, 0);


					indexCount += sphereMesh->GetIndexCount();

					++i;
				} while (i < numLightSources && lightSources[i].type == lightType);
				break;
		}
	}
}

void ScratchEngine::Rendering::RenderingEngine::DrawDeferred(ID3D11ShaderResourceView* lightBuffer, ID3D11RenderTargetView* backBuffer, ID3D11DepthStencilView* depthStencilView)
{
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
}

void ScratchEngine::Rendering::RenderingEngine::RenderShadowMap(LightSource* light, Renderable* renderables, int numRenderables)
{
	Shadow* shadow = light->shadow;


	D3D11_VIEWPORT viewport = {};
	viewport.Width = shadow->width;
	viewport.Height = shadow->height;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;


	deviceContext->ClearDepthStencilView(shadow->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	deviceContext->RSSetState(rsShadow);
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->PSSetShader(nullptr, nullptr, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, shadow->depthStencilView);
	

	vsDepthOnly->SetMatrix4x4("viewProjection", light->shadowViewProjection);

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
}

void ScratchEngine::Rendering::RenderingEngine::SetShadowMap(ShadowMap * _shadow)
{
	shadow = _shadow;
}

void ScratchEngine::Rendering::RenderingEngine::RenderCubeMap(CubeMap* cubeMap, Viewer* viewer)
{
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
}
