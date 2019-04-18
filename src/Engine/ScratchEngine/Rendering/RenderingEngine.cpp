#include "../Core/GameObject.h"
#include "../Core/Global.h"

#include "RenderingEngine.h"
#include "SimpleShader.h"

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::singleton = nullptr;

ScratchEngine::Rendering::RenderingEngine* ScratchEngine::Rendering::RenderingEngine::GetSingleton()
{
	if (!singleton)
		Initialize();

	return singleton;
}

void ScratchEngine::Rendering::RenderingEngine::Initialize(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables, i32 defaultNumViews)
{
	singleton = new RenderingEngine(maxNumMeshes, maxNumMeshes, defaultNumRenderables, defaultNumViews);
}

void ScratchEngine::Rendering::RenderingEngine::Stop()
{
	if (singleton)
		delete singleton;
}

ScratchEngine::Rendering::RenderingEngine::RenderingEngine(i32 maxNumMaterials, i32 maxNumMeshes, i32 defaultNumRenderables, i32 defaultNumCameraProxies) : materialAllocator(maxNumMeshes), meshAllocator(maxNumMeshes), renderableAllocator(defaultNumRenderables), viewerAllocator(defaultNumCameraProxies)
{
	rendererList = nullptr;
	cameraList = nullptr;
	lightList = nullptr;
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

	if (next)
		next = previous;

	if (renderer == rendererList)
		rendererList = next;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveCamera(Camera * camera)
{
	if (!rendererList)
		return;

	Camera* next = camera->next;
	Camera* previous = camera->previous;

	if (previous)
		previous->next = next;

	if (next)
		next = previous;

	if (camera == cameraList)
		cameraList = next;
}

void ScratchEngine::Rendering::RenderingEngine::RemoveLight(Light* light)
{
	if (!rendererList)
		return;

	Light* next = light->next;
	Light* previous = light->previous;

	if (previous)
		previous->next = next;

	if (next)
		next = previous;

	if (light == lightList)
		lightList = next;
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
			lightSource.direction = XMFLOAT3(0, -1.0f, 0);

			XMStoreFloat3(&lightSource.position, light->GetPosition());
			//XMStoreFloat3(&lightSource.direction, static_cast<DirectionalLight*>(light)->GetDirection());
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

void ScratchEngine::Rendering::RenderingEngine::PerformZPrepass(SimpleVertexShader* shader, ID3D11DeviceContext* context)
{
	context->PSSetShader(nullptr, nullptr, 0);

	Viewer& viewer = viewerAllocator[cameraList->viewer];

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewer.projectionMatrix, viewer.viewMatrix);

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Renderable& renderable = renderableAllocator[j];

		shader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		shader->SetMatrix4x4("world", renderable.worldMatrix);

		shader->CopyAllBufferData();
		shader->SetShader();

		Mesh* mesh = renderable.mesh;

		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		UINT indexCount = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

		indexCount += mesh->GetIndexCount();

		++j;
	}
}

void ScratchEngine::Rendering::RenderingEngine::DrawForward(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* shadowMap = shadow->getShadowSRV();

	Viewer& viewer = viewerAllocator[cameraList->viewer];
	
	XMMATRIX viewMatrix = viewer.viewMatrix;
	XMMATRIX projectionMatrix = viewer.projectionMatrix;
	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(projectionMatrix, viewMatrix);
	XMVECTOR cameraPosition = viewer.position;

	XMFLOAT4X4 shadowViewMatrix;
	XMFLOAT4X4 shadowProjectionMatrix;

	XMMATRIX shadowView = XMMatrixLookToLH(
		XMVectorSet(0, 10, 0, 0),
		XMVectorSet(0, -1, 1, 0), 
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shadowView));

	XMMATRIX shadowProjection = XMMatrixOrthographicLH(
		10,
		10,
		0.1f,
		50);
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shadowProjection));

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Material* material = renderableAllocator[j].material;
		SimpleVertexShader* vertexShader = material->GetVertexShader();
		SimplePixelShader* pixelShader = material->GetPixelShader();

		pixelShader->SetData("light", lightSourceAllocator.GetMemoryAddress(), sizeof(LightSource));
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
			vertexShader->SetMatrix4x4("shadowView", shadowViewMatrix);
			vertexShader->SetMatrix4x4("shadowProjection", shadowProjectionMatrix);

			vertexShader->CopyAllBufferData();
			vertexShader->SetShader();

			Mesh* mesh = renderable.mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

			indexCount += mesh->GetIndexCount();

			++j;
		} while (j < renderableAllocator.GetNumAllocated() && renderableAllocator[j].material == material);

		++j;
	}
}

bool ScratchEngine::Rendering::RenderingEngine::RenderShadowMap(ID3D11DeviceContext* context) {
	context->OMSetRenderTargets(0, 0, shadow->getShadowDSV());
	context->ClearDepthStencilView(shadow->getShadowDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadow->getRasterizerState());

	XMFLOAT4X4 shadowViewMatrix;
	XMFLOAT4X4 shadowProjectionMatrix;

	XMMATRIX shadowView = XMMatrixLookToLH(
		XMVectorSet(0, 10, 0, 0),
		XMVectorSet(0, -1, 1, 0), 
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shadowView));

	XMMATRIX shadowProjection = XMMatrixOrthographicLH(
		10,
		10,
		0.1f,
		50);
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shadowProjection));

	SimpleVertexShader* shader = shadow->getShadowShader();

	shader->SetShader();
	shader->SetMatrix4x4("shadowView", shadowViewMatrix);
	shader->SetMatrix4x4("shadowProjection", shadowProjectionMatrix);

	context->PSSetShader(0, 0, 0);

	u32 stride = sizeof(Vertex);
	u32 offset = 0;

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Renderable& renderable = renderableAllocator[j];

		Mesh* mesh = renderable.mesh;

		ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

		context->IASetVertexBuffers(
			0,
			1,
			&vertexBuffer,
			&stride,
			&offset
		);

		context->IASetIndexBuffer(
			indexBuffer,
			DXGI_FORMAT_R32_UINT, 
			0
		);

		//set up constant buffer
		shader->SetMatrix4x4("world", renderable.worldMatrix);
		shader->CopyAllBufferData();
		
		//......

		context->DrawIndexed(mesh->GetIndexCount(), 0, 0);


		++j;
	}
	
	return true;
}

void ScratchEngine::Rendering::RenderingEngine::SetShadowMap(ShadowMap * _shadow) {
	shadow = _shadow;
}

void ScratchEngine::Rendering::RenderingEngine::RenderCubeMap(ID3D11DeviceContext * context, CubeMap* cubeMap) {
	ID3D11Buffer* cubeVB = cubeMap->getVB();
	ID3D11Buffer* cubeIB = cubeMap->getIB();

	u32 stride = sizeof(Vertex);
	u32 offset = 0;

	context->IASetVertexBuffers(0, 1, &cubeVB, &stride, &offset);
	context->IASetIndexBuffer(cubeIB, DXGI_FORMAT_R32_UINT, 0);

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

	context->RSSetState(cubeMap->getRS());
	context->OMSetDepthStencilState(cubeMap->getDSS(), 0);

	context->DrawIndexed(cubeMap->getIndexCount(), 0, 0);
}
