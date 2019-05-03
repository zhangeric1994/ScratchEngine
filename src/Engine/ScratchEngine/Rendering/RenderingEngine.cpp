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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&depthStencilDesc, &dssLessEqual);
}

ScratchEngine::Rendering::RenderingEngine::~RenderingEngine()
{
	singleton = nullptr;

	if (vsDepthOnly)
		delete vsDepthOnly;

	if (dssLessEqual)
		dssLessEqual->Release();
}

void ScratchEngine::Rendering::RenderingEngine::PerformZPrepass(Viewer* viewer, Renderable* renderables, int numRenderables)
{
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	XMMATRIX viewProjectionMatrix = XMMatrixMultiply(viewer->projectionMatrix, viewer->viewMatrix);
	
	vsDepthOnly->SetShader();

	for (int j = 0; j < numRenderables; ++j)
	{
		Renderable& renderable = renderables[j];

		vsDepthOnly->SetMatrix4x4("viewProjection", viewProjectionMatrix);
		vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
		vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

		vsDepthOnly->CopyAllBufferData();

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

		pixelShader->SetFloat4("tint", material->GetTint());
		pixelShader->SetData("light", lightSources, numLightSources * sizeof(LightSource));
		pixelShader->SetFloat3("cameraPosition", cameraPosition);

		if (material->HasTexture()) {
			pixelShader->SetShaderResourceView("diffuseMap", material->getTexture());
		}

		pixelShader->SetSamplerState("basicSampler", material->getSampler());

		if (material->HasNormalMap()) {
			pixelShader->SetShaderResourceView("normalMap", material->getNormalMap());
		}

		//if (material->HasShadowMap()) {
			pixelShader->SetShaderResourceView("ShadowMap", shadow->getShadowSRV());
		//}
		
		pixelShader->SetSamplerState("shadowSampler", shadow->getSampler());

		if (material->HasRoughnessMap()) {
			pixelShader->SetShaderResourceView("roughnessMap", material->getRoughnessMap());
		}

		if (material->HasMetalnessMap()) {
			pixelShader->SetShaderResourceView("metalnessMap", material->getMetalnessMap());
		}

		vertexShader->SetShader();

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
			Renderable& renderable = renderables[j];

			vertexShader->SetMatrix4x4("view", viewMatrix);
			vertexShader->SetMatrix4x4("projection", projectionMatrix);
			vertexShader->SetMatrix4x4("viewProjection", viewProjectionMatrix);
			vertexShader->SetMatrix4x4("world", renderable.worldMatrix);
			vertexShader->SetMatrix4x4("shadowViewProjection", shadowViewProjectionMat);
			vertexShader->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

			vertexShader->CopyAllBufferData();

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

bool ScratchEngine::Rendering::RenderingEngine::RenderShadowMap(Renderable* renderables, int numToDraw)
{
	deviceContext->PSSetShader(nullptr, nullptr, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, shadow->getShadowDSV());

	deviceContext->ClearDepthStencilView(shadow->getShadowDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->RSSetState(shadow->getRasterizerState());

	vsDepthOnly->SetShader();

	for (int j = 0; j < numToDraw; ++j)
	{
		Renderable& renderable = renderables[j];

		vsDepthOnly->SetMatrix4x4("viewProjection", shadowViewProjectionMat);
		vsDepthOnly->SetMatrix4x4("world", renderable.worldMatrix);
		vsDepthOnly->SetData("gBoneTransforms", renderable.bones, sizeof(XMMATRIX) * MAX_NUM_BONES_PER_MODEL);

		vsDepthOnly->CopyAllBufferData();

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
	
	return true;
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
