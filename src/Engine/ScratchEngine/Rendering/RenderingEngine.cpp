#include "../Core/GameObject.h"
#include "../Core/Global.h"

#include "RenderingEngine.h"
#include "SimpleShader.h"

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::singleton = nullptr;


ScratchEngine::Rendering::RenderingEngine::RenderingEngine(RenderingEngineConfig config) : materialAllocator(config.maxNumMeshes), meshAllocator(config.maxNumMeshes), renderableAllocator(config.initialNumRenderables), viewerAllocator(config.initialNumRenderables), lightSourceAllocator(config.initialNumLightSources)
{
	assert(config.device);
	device = config.device;

	assert(config.deviceContext);
	deviceContext = config.deviceContext;

	rendererList = nullptr;
	cameraList = nullptr;
	lightList = nullptr;
	shadow = nullptr;
	hasZPrepass = false;


	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	std::string spath = std::string(buffer).substr(0, pos).c_str();
	std::wstring wpath = std::wstring(spath.begin(), spath.end());

	vsDepthOnly = new SimpleVertexShader(device, deviceContext);
	vsDepthOnly->LoadShaderFile((wpath + std::wstring(L"/vs_zprepass.cso")).c_str());

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, &dssLessEqual);
}

ScratchEngine::Rendering::RenderingEngine::~RenderingEngine()
{
	singleton = nullptr;
}

void ScratchEngine::Rendering::RenderingEngine::AddRenderer(Renderer* renderer)
{
	renderer->next = rendererList;
	renderer->previous = nullptr;

	if (rendererList)
		rendererList->previous = renderer;
	
	rendererList = renderer;
}

void ScratchEngine::Rendering::RenderingEngine::AddCamera(Camera* camera)
{
	camera->next = cameraList;
	camera->previous = nullptr;

	if (cameraList)
		cameraList->previous = camera;
	
	cameraList = camera;
}

void ScratchEngine::Rendering::RenderingEngine::AddLight(Light* light)
{
	light->next = lightList;
	light->previous = nullptr;

	if (lightList)
		lightList->previous = light;
	
	lightList = light;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveRenderer(Renderer* renderer)
{
	if (!rendererList)
		return;

	Renderer* next = renderer->next;
	Renderer* previous = renderer->previous;

	if (previous)
		previous->next = next;
	else
		rendererList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveCamera(Camera * camera)
{
	if (!rendererList)
		return;

	Camera* next = camera->next;
	Camera* previous = camera->previous;

	if (previous)
		previous->next = next;
	else
		cameraList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveLight(Light* light)
{
	if (!rendererList)
		return;

	Light* next = light->next;
	Light* previous = light->previous;

	if (previous)
		previous->next = next;
	else
		lightList = next;

	if (next)
		next->previous = previous;
}

void ScratchEngine::Rendering::RenderingEngine::DestroyRenderable(i32 id)
{
	// renderableAllocator.Free(id);
}

void ScratchEngine::Rendering::RenderingEngine::DestroyViewer(i32 id)
{
	viewerAllocator.Free(id);
}

void ScratchEngine::Rendering::RenderingEngine::UpdateRenderables()
{
	renderableAllocator.Flush();

	for (Renderer* renderer = rendererList; renderer; renderer = renderer->next)
	{
		if (renderer->IsActive())
		{
			Renderable& renderable = renderableAllocator[renderableAllocator.Allocate()];

			renderable.worldMatrix = XMMatrixTranspose(renderer->GetGameObject()->GetWorldMatrix());
			renderable.material = renderer->material;
			renderable.mesh = renderer->mesh;
		}
	}
}

void ScratchEngine::Rendering::RenderingEngine::UpdateViewers()
{
	f32 screenRatio = ScratchEngine::Global::GetScreenRatio();

	for (Camera* camera = cameraList; camera; camera = camera->next)
	{
		if (camera->IsActive())
		{
			if (camera->viewer == null_index)
				camera->viewer = viewerAllocator.Allocate();

			Viewer& viewer = viewerAllocator[camera->viewer];

			GameObject* gameObject = camera->GetGameObject();

			XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(gameObject->GetRotation());

			viewer.position = gameObject->GetPosition();
			//viewer.position = XMVectorSet(0, 0, -5.0f, 0);
			viewer.viewMatrix = XMMatrixTranspose(XMMatrixLookToLH(viewer.position, XMVector3Transform({ 0, 0, 1 }, rotationMatrix), { 0, 1, 0 }));
			viewer.projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(camera->fov, screenRatio, camera->nearZ, camera->farZ));
		}
		else
			RenderingEngine::GetSingleton()->DestroyViewer(camera->viewer);
	}
}

void ScratchEngine::Rendering::RenderingEngine::UpdateLightSources()
{
	lightSourceAllocator.Flush();

	for (Light* light = lightList; light; light = light->next)
	{
		if (light->IsActive())
		{
			LightSource& lightSource = lightSourceAllocator[lightSourceAllocator.Allocate()];
			lightSource.ambientColor = light->ambientColor;
			lightSource.diffuseColor = light->diffuseColor;
			lightSource.type = light->type;
			lightSource.range = 0;

			XMStoreFloat3(&lightSource.position, light->GetGameObject()->GetPosition());
			XMStoreFloat3(&lightSource.direction, static_cast<DirectionalLight*>(light)->GetGameObject()->GetForwardVector());

			//if (light->DoCastShadow())
			//	lightSource.shadowMapID = null_index;
			//else
			//	lightSource.shadowMapID = null_index;
		}
	}
}

void ScratchEngine::Rendering::RenderingEngine::SortRenderables()
{
	renderableAllocator.Sort([](Renderable a, Renderable b)
	{
		uptr ma = reinterpret_cast<uptr>(a.material);
		uptr mb = reinterpret_cast<uptr>(b.material);

		return ma < mb;
	});
}

void ScratchEngine::Rendering::RenderingEngine::PerformZPrepass()
{
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	Viewer& viewer = viewerAllocator[cameraList->viewer];

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewer.projectionMatrix, viewer.viewMatrix);

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Renderable& renderable = renderableAllocator[j];

		vsDepthOnly->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);

		vsDepthOnly->CopyAllBufferData();
		vsDepthOnly->SetShader();

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

		++j;
	}

	hasZPrepass = true;
}

void ScratchEngine::Rendering::RenderingEngine::DrawForward()
{
	if (hasZPrepass)
		deviceContext->OMSetDepthStencilState(dssLessEqual, 0);

	ID3D11ShaderResourceView* shadowMap = shadow->getShadowSRV();

	Viewer& viewer = viewerAllocator[cameraList->viewer];
	
	XMMATRIX viewMatrix = viewer.viewMatrix;
	XMMATRIX projectionMatrix = viewer.projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
	XMVECTOR cameraPosition = viewer.position;

	XMMATRIX shadowView = XMMatrixLookToLH(XMVectorSet(0, 50, 0, 0), XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 1, 0, 0));
	XMMATRIX shadowProjection = XMMatrixOrthographicLH(128, 128, 0.1f, 150);

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Material* material = renderableAllocator[j].material;
		SimpleVertexShader* vertexShader = material->GetVertexShader();
		SimplePixelShader* pixelShader = material->GetPixelShader();

		pixelShader->SetFloat4("tint", material->GetTint());
		pixelShader->SetData("light", lightSourceAllocator.GetMemoryAddress(), lightSourceAllocator.GetNumAllocated() * sizeof(LightSource));
		pixelShader->SetFloat4("cameraPosition", cameraPosition);

		if (material->HasTexture()) {
			pixelShader->SetShaderResourceView("diffuseTexture", material->getTexture());
		}

		pixelShader->SetSamplerState("basicSampler", material->getSampler());

		if (material->HasNormalMap()) {
			pixelShader->SetShaderResourceView("normalMap", material->getNormalMap());
		}

		if (material->HasShadowMap()) {
			pixelShader->SetShaderResourceView("ShadowMap", shadow->getShadowSRV());
		}
		
		pixelShader->SetSamplerState("shadowSampler", material->getSampler());

		if (material->HasRoughnessMap()) {
			pixelShader->SetShaderResourceView("roughnessMap", material->getRoughnessMap());
		}

		if (material->HasMetalnessMap()) {
			pixelShader->SetShaderResourceView("metalnessMap", material->getMetalnessMap());
		}

		pixelShader->SetInt("hasTexture", material->HasTexture());
		pixelShader->SetInt("hasNormalMap", material->HasNormalMap());
		pixelShader->SetInt("hasShadowMap", material->HasShadowMap());
		pixelShader->SetInt("hasRoughnessMap", material->HasRoughnessMap());
		pixelShader->SetInt("hasMetalnessMap", material->HasMetalnessMap());

		pixelShader->CopyAllBufferData();
		pixelShader->SetShader();

		u32 stride = sizeof(Vertex);
		u32 offset = 0;
		u32 indexCount = 0;

		do
		{
			Renderable& renderable = renderableAllocator[j];

			vertexShader->SetMatrix4x4("view", viewMatrix);
			vertexShader->SetMatrix4x4("projection", projectionMatrix);
			vertexShader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
			vertexShader->SetMatrix4x4("world", renderable.worldMatrix);
			vertexShader->SetMatrix4x4("shadowView", shadowView);
			vertexShader->SetMatrix4x4("shadowProjection", shadowProjection);

			vertexShader->CopyAllBufferData();
			vertexShader->SetShader();

			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

			deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			deviceContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);

			indexCount += mesh->GetIndexCount();

			++j;
		} while (j < renderableAllocator.GetNumAllocated() && renderableAllocator[j].material == material);
	}
}

bool ScratchEngine::Rendering::RenderingEngine::RenderShadowMap()
{
	deviceContext->OMSetRenderTargets(0, nullptr, shadow->getShadowDSV());
	deviceContext->ClearDepthStencilView(shadow->getShadowDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->RSSetState(shadow->getRasterizerState());

	XMMATRIX shadowView = XMMatrixLookToLH(XMVectorSet(0, 50, 0, 0), XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 1, 0, 0));
	XMMATRIX shadowProjection = XMMatrixOrthographicLH(128, 128, 0.1f, 150);

	SimpleVertexShader* shader = vsDepthOnly;

	shader->SetShader();

	deviceContext->PSSetShader(0, 0, 0);

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(shadowProjection, shadowView);

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Renderable& renderable = renderableAllocator[j];

		shader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		shader->SetMatrix4x4("world", renderable.worldMatrix);

		shader->CopyAllBufferData();

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

		++j;
	}

	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	
	return true;
}

void ScratchEngine::Rendering::RenderingEngine::SetShadowMap(ShadowMap * _shadow)
{
	shadow = _shadow;
}

void ScratchEngine::Rendering::RenderingEngine::RenderCubeMap(CubeMap* cubeMap)
{
	ID3D11Buffer* cubeVB = cubeMap->getVB();
	ID3D11Buffer* cubeIB = cubeMap->getIB();

	u32 stride = sizeof(Vertex);
	u32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &cubeVB, &stride, &offset);
	deviceContext->IASetIndexBuffer(cubeIB, DXGI_FORMAT_R32_UINT, 0);

	Viewer& viewer = viewerAllocator[cameraList->viewer];

	XMMATRIX viewMatrix = viewer.viewMatrix;
	XMMATRIX projectionMatrix = viewer.projectionMatrix;

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
