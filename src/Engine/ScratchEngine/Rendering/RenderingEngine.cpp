#include "../Core/GameObject.h"
#include "../Core/Global.h"

#include "RenderingEngine.h"
#include "SimpleShader.h"

static bool ScratchEngine::Rendering::SortRenderables(Renderable a, Renderable b)
{
	uptr ma = reinterpret_cast<uptr>(a.material);
	uptr mb = reinterpret_cast<uptr>(b.material);

	return ma < mb;
}

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
	else
		rendererList = renderer;
}

void ScratchEngine::Rendering::RenderingEngine::AddCamera(Camera* camera)
{
	camera->next = cameraList;
	camera->previous = nullptr;

	if (cameraList)
		cameraList->previous = camera;
	else
		cameraList = camera;
}

void ScratchEngine::Rendering::RenderingEngine::AddLight(Light* light)
{
	light->next = lightList;
	light->previous = nullptr;

	if (lightList)
		lightList->previous = light;
	else
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
	for (Renderer* renderer = rendererList; renderer; renderer = renderer->next)
	{
		if (renderer->isEnabled)
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
		if (camera->isEnabled)
		{
			if (camera->viewer == null_index)
				camera->viewer = viewerAllocator.Allocate();

			Viewer& viewer = viewerAllocator[camera->viewer];

			GameObject* gameObject = camera->GetGameObject();

			XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(gameObject->GetRotation());

			viewer.position = gameObject->GetPosition();
			viewer.viewMatrix = XMMatrixLookToLH(viewer.position, XMVector3Transform(XMVectorSet(0, 0, 1, 0), rotationMatrix), XMVector3Transform(XMVectorSet(0, 1, 0, 0), rotationMatrix));
			viewer.projectionMatrix = XMMatrixPerspectiveFovLH(camera->fov, screenRatio, camera->nearZ, camera->farZ);;
		}
		else
			RenderingEngine::GetSingleton()->DestroyViewer(camera->viewer);
	}
}

void ScratchEngine::Rendering::RenderingEngine::UpdateLightSources()
{
	for (Light* light = lightList; light; light = light->next)
	{
		if (light->isEnabled)
		{
			LightSource& lightSource = lightSourceAllocator[lightSourceAllocator.Allocate()];
			lightSource.ambientColor = light->ambientColor;
			lightSource.diffuseColor = light->diffuseColor;
			lightSource.type = light->type;
			lightSource.range = 0;

			XMStoreFloat3(&lightSource.position, light->GetPosition());
			XMStoreFloat3(&lightSource.direction, static_cast<DirectionalLight*>(light)->GetDirection());
		}
	}
}

void ScratchEngine::Rendering::RenderingEngine::SortRenderables()
{
	renderableAllocator.Sort(ScratchEngine::Rendering::SortRenderables);
}

void ScratchEngine::Rendering::RenderingEngine::DrawForward(ID3D11DeviceContext* context)
{
	Viewer& viewer = viewerAllocator[cameraList->viewer];
	XMMATRIX viewMatrix = viewer.viewMatrix;
	XMMATRIX projectionMatrix = viewer.projectionMatrix;

	i32 j = 0;

	while (j < renderableAllocator.GetNumAllocated())
	{
		Renderable& renderable = renderableAllocator[j];
		Material* material = renderable.material;
		SimpleVertexShader* vertexShader = material->GetVertexShader();
		SimplePixelShader* pixelShader = material->GetPixelShader();

		vertexShader->SetMatrix4x4("world", renderable.worldMatrix);
		vertexShader->SetMatrix4x4("view", viewMatrix);
		vertexShader->SetMatrix4x4("projection", projectionMatrix);

		pixelShader->SetData("light", &lightSourceAllocator[0], sizeof(LightSource));
		
		vertexShader->CopyAllBufferData();
		pixelShader->CopyAllBufferData();

		vertexShader->SetShader();
		pixelShader->SetShader();

		u32 indexCount = 0;
		u32 stride = sizeof(Vertex);
		u32 offset = 0;

		do
		{
			Mesh* mesh = renderableAllocator[j].mesh;

			ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
			ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			context->DrawIndexed(mesh->GetIndexCount(), 0, 0);

			indexCount += mesh->GetIndexCount();
		} while (renderableAllocator[++j].material == material);

		++j;
	}
}
