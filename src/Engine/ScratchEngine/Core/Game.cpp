#include <string>
#include <WICTextureLoader.h>

#include "../Physics/PhysicsEngine.h"
#include "../Rendering/RenderingEngine.h"
#include "../Rendering/Mesh.h"

#include "../Dummy.h"
#include "../Mob.h"

#include "Game.h"
#include "Global.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Scene.h"
#include "Transform.h"

using namespace ScratchEngine::Physics;
using namespace ScratchEngine::Rendering;


__forceinline void ScratchEngine::Game::__RenderShadows(RenderingEngine* renderingEngine, Scene* scene, Viewer* viewer, D3D11_VIEWPORT* viewport)
{
	for (int i = 0; i < scene->lightSourceAllocator.GetNumAllocated(); ++i)
	{
		LightSource& lightSource = scene->lightSourceAllocator[i];

		if (lightSource.shadow)
		{
			switch (lightSource.type)
			{
			case LightType::DIRECTIONAL:
				{
					XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(camera->GetLocalPosition(), XMLoadFloat3(&lightSource.direction), XMVectorSet(0, 1, 0, 0)));
					XMMATRIX shadowProjection = XMMatrixTranspose(XMMatrixOrthographicLH(30, 30, 0.1f, 100));

					lightSource.shadowViewProjection[0] = XMMatrixMultiply(shadowProjection, shadowView);

					CSMConfig config = {};
					config.light = &lightSource;
					config.viewer = viewer;
					config.viewport = viewport;
					config.depthStencilView = depthStencilView;
					config.numCascades = lightSource.shadow->GetNumCascades();
					config.selectionFactor = 0.8f;
					config.powerExponent = 2.0f;

					renderingEngine->RenderCSM(config, scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated());
					//renderingEngine->RenderCSMDebug(config, scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), GBufferAlbedoRTV);
				}

				break;


			case LightType::AMBIENT:
				if (renderingMode == 9)
					lightSource.shadow = nullptr;
				else
				{
					lightSource.shadowProjection = scene->viewerAllocator[0].projectionMatrix;

					renderingEngine->RenderSSAO(&lightSource, depthSRV, GBufferNormalsSRV);
				}

				break;


			default:
				renderingEngine->RenderShadowMap(&lightSource, scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated());
			}
		}
	}
}


void CreateSRVAndRTV(ID3D11Device2* device, ID3D11ShaderResourceView** srv, ID3D11RenderTargetView** rtv, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
{
	// Set up the texture itself
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.Format = format;
	texDesc.MipLevels = 1;
	texDesc.Width = 1600;
	texDesc.Height = 900;
	texDesc.SampleDesc.Count = 1;

	// Actually create the texture
	ID3D11Texture2D* texture;
	device->CreateTexture2D(&texDesc, 0, &texture);


	// Create the shader resource view for this texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture, &srvDesc, srv);
	

	// Make a render target view desc and RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(texture, &rtvDesc, rtv);
	

	// Clean up extra texture ref
	texture->Release();
}


Material* ScratchEngine::Game::greenMaterial = nullptr;
Material* ScratchEngine::Game::redMaterial = nullptr;

ScratchEngine::Game::Game(HINSTANCE hInstance, char* name) : DXCore(hInstance, name, 1600, 900, true), frameBarrier(2)
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	vsZPrepass = nullptr;

	greenMaterial = nullptr;
	redMaterial = nullptr;

	zPrepassDepthStencilState = nullptr;

	GBufferAlbedoSRV = nullptr;
	GBufferAlbedoRTV = nullptr;
	GBufferNormalsSRV = nullptr;
	GBufferNormalsRTV = nullptr;
	GBufferDepthSRV = nullptr;
	GBufferDepthRTV = nullptr;
	GBufferMaterialSRV = nullptr;
	GBufferMaterialRTV = nullptr;
	DeferredLightBufferSRV = nullptr;
	DeferredLightBufferRTV = nullptr;
	
	sphereMesh = nullptr;
	cubeMesh = nullptr;

	sampler = 0;
	texture = 0;
	normalMap = 0;
	metalnessMap = 0;
	roughnessMap = 0;

	isMouseDownL = false;
	isMouseDownR = false;
	lastInputTime = 0.0f;


	Global::SetScreenRatio(1600.0f / 900.0f);


#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

ScratchEngine::Game::~Game()
{
	if (vertexShader)
		delete vertexShader;

	if (vsZPrepass)
		delete vsZPrepass;

	if (vsSkeleton)
		delete vsSkeleton;

	if (pixelShader)
		delete pixelShader;

	if (psPBR)
		delete psPBR;

	if (psBlinnPhong)
		delete psBlinnPhong;

	if (pbrMaterial)
		delete pbrMaterial;

	if (greenMaterial)
		delete greenMaterial;

	if (redMaterial)
		delete redMaterial;

	if (skeletonMaterial)
		delete skeletonMaterial;

	if (zPrepassDepthStencilState)
		zPrepassDepthStencilState->Release();


	if (cubeMesh)
		delete cubeMesh;
	
	if (sphereMesh)
		delete sphereMesh;

	if (texture)
		texture->Release();

	if (sampler)
		sampler->Release();

	if (normalMap)
		normalMap->Release();

	if (roughnessMap)
		roughnessMap->Release();

	if (metalnessMap)
		metalnessMap->Release();


	RenderingEngine::Terminate();

	delete scene;
}

void ScratchEngine::Game::Initialize()
{
	InputManager::Initialize();
	PhysicsEngine::Initialize();
	RenderingEngine::Initialize(device, context);

	LoadShaders();
	CreateMatrces();
	CreateBasicGeometry();
	CreateAllMaps();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	camX = 0;
	camY = 0;


	scene = Scene::GetCurrentScene();


	jobSystem.ActivateThreads(2);
}

void ScratchEngine::Game::LoadShaders()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");
	
	std::string spath = std::string(buffer).substr(0, pos).c_str();
	std::wstring wpath = std::wstring(spath.begin(), spath.end());

	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile((wpath + std::wstring(L"/VertexShader.cso")).c_str());

	vsSkeleton = new SimpleVertexShader(device, context);
	vsSkeleton->LoadShaderFile((wpath + std::wstring(L"/VS_Skeleton.cso")).c_str());

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile((wpath + std::wstring(L"/PixelShader.cso")).c_str());

	psPBR = new SimplePixelShader(device, context);
	psPBR->LoadShaderFile((wpath + std::wstring(L"/PixelShaderPBR.cso")).c_str());

	psBlinnPhong = new SimplePixelShader(device, context);
	psBlinnPhong->LoadShaderFile((wpath + std::wstring(L"/PS_BlinnPhong.cso")).c_str());


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &sampler);


	D3D11_DEPTH_STENCIL_DESC dssDesc = {};
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dssDesc, &zPrepassDepthStencilState);


	CreateSRVAndRTV(device, &GBufferAlbedoSRV, &GBufferAlbedoRTV);
	CreateSRVAndRTV(device, &GBufferNormalsSRV, &GBufferNormalsRTV, DXGI_FORMAT_R16G16B16A16_UNORM);
	CreateSRVAndRTV(device, &GBufferDepthSRV, &GBufferDepthRTV, DXGI_FORMAT_R16G16B16A16_FLOAT);
	CreateSRVAndRTV(device, &GBufferMaterialSRV, &GBufferMaterialRTV);
	CreateSRVAndRTV(device, &DeferredLightBufferSRV, &DeferredLightBufferRTV, DXGI_FORMAT_R16G16B16A16_FLOAT); // Needs to go above 1!!!


	sky = new TextureCube(L"../Assets/Textures/CubeMaps/Skybox2.dds", &samplerDesc);
}

void ScratchEngine::Game::CreateAllMaps()
{
}

void ScratchEngine::Game::CreateMatrces()
{
}

void ScratchEngine::Game::CreateBasicGeometry()
{
	if (FAILED(CreateWICTextureFromFile(device, context, L"../Assets/Textures/chipped-paint-metal/Mossy_driveway_01_2K_Base_Color.png", 0, &texture)))
		printf("load albedo texture error");

	if (FAILED(CreateWICTextureFromFile(device, context, L"../Assets/Textures/chipped-paint-metal/Mossy_driveway_01_2K_Normal.png", 0, &normalMap)))
		printf("load normal map error");

	//load roughness map
	if (FAILED(CreateWICTextureFromFile(device, context, L"../Assets/Textures/chipped-paint-metal/Mossy_driveway_01_2K_Roughness.png", 0, &roughnessMap)))
		printf("load roughness map error");

	//load metalness map
	if (FAILED(CreateWICTextureFromFile(device, context, L"../Assets/Textures/chipped-paint-metal/chipped-paint-metal-metal.pn", 0, &metalnessMap)))
		printf("load metalness map failed");


	sphereMesh = new Mesh(device, (char*)"../Assets/Models/sphere.obj");
	cubeMesh = new Mesh(device, (char*)"../Assets/Models/cube.obj");

	mobModel = new Model(device, "../Assets/Models/Mob/nightshade_j_friedrich.fbx");
	mobModel->LoadAnimation("../Assets/Models/Mob/Idle_0.fbx");					// 1
	mobModel->LoadAnimation("../Assets/Models/Mob/Idle_1.fbx");					// 2
	mobModel->LoadAnimation("../Assets/Models/Mob/Idle_2.fbx");					// 3
	mobModel->LoadAnimation("../Assets/Models/Mob/Reaction_0.fbx");				// 4
	mobModel->LoadAnimation("../Assets/Models/Mob/Reaction_1.fbx");				// 5
	mobMaterial = new Material(vsSkeleton, psPBR, sampler, "../Assets/Models/Mob/nightshade_j_friedrich.fbx");

	model = new Model(device, "../Assets/Models/Pack/vampire_a_lusth.fbx");
	model->LoadAnimation("../Assets/Models/Pack/Standing Idle 01.fbx");				// 1
	model->LoadAnimation("../Assets/Models/Pack/Standing Jump.fbx");			// 2
	model->LoadAnimation("../Assets/Models/Pack/Turn.fbx");							// 3 
	model->LoadAnimation("../Assets/Models/Pack/Standing Walk Forward.fbx");		// 4
	model->LoadAnimation("../Assets/Models/Pack/Standing Walk Back.fbx");			// 5
	model->LoadAnimation("../Assets/Models/Pack/Standing Walk Left.fbx");			// 6
	model->LoadAnimation("../Assets/Models/Pack/Standing Walk Right.fbx");			// 7
	model->LoadAnimation("../Assets/Models/Pack/Standing Run Forward.fbx");			// 8		
	model->LoadAnimation("../Assets/Models/Pack/Standing Run Back.fbx");			// 9
	model->LoadAnimation("../Assets/Models/Pack/Standing Run Left.fbx");			// 10
	model->LoadAnimation("../Assets/Models/Pack/Standing Run Right.fbx");			// 11
	model->LoadAnimation("../Assets/Models/Pack/Standing Idle 02.fbx");				// 12
	model->LoadAnimation("../Assets/Models/Pack/Standing Idle 03.fbx");				// 13

	model->LoadAnimation("../Assets/Models/Pack/Boxing_0.fbx");				// 14
	model->LoadAnimation("../Assets/Models/Pack/Boxing_1.fbx");				// 15
	model->LoadAnimation("../Assets/Models/Pack/Boxing_2.fbx");				// 16
	model->LoadAnimation("../Assets/Models/Pack/Kick_0.fbx");				// 17

	pbrMaterial = new Material(vertexShader, psPBR, sampler);
	pbrMaterial->SetTexture(texture);
	pbrMaterial->SetMetalnessMap(metalnessMap);
	pbrMaterial->SetNormalMap(normalMap);
	pbrMaterial->SetRoughnessMap(roughnessMap);

	greenMaterial = new Material(vertexShader, pixelShader, nullptr);
	greenMaterial->SetTint(0, 1, 0);

	redMaterial = new Material(vertexShader, pixelShader, nullptr);
	redMaterial->SetTint(1, 0, 0);

	skeletonMaterial = new Material(vsSkeleton, psPBR, sampler, "../Assets/Models/Pack/vampire_a_lusth.fbx");
	

	GameObject* directionalLightObject = new GameObject();
	directionalLightObject->SetLocalRotation(55, 0, 0);
	directionalLight = directionalLightObject->AddComponent<DirectionalLight>(XMVectorSet(0.5f, 0.5f, 0.3f, 1.0f), 10.0f);
	directionalLight->EnableShadowCasting(6);

	GameObject* pointLightObject = new GameObject();
	pointLightObject->SetLocalPosition(3, 2, 10);
	pointLight = pointLightObject->AddComponent<PointLight>(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), 10.0f, 10.0f);
	pointLight->EnableShadowCasting();

	GameObject* ambientLightObject = new GameObject();
	ambientLight = ambientLightObject->AddComponent<AmbientLight>();
	ambientLight->EnableShadowCasting();


	//go1 = new GameObject();
	//go1->SetName("1");
	//go1->SetPosition(0, 0, 15);
	//go1->SetLocalRotation(45, 0, 90);
	//go1->SetLocalScale(1, 2, 1);
	//go1->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go1->AddComponent<BoxCollider>();


	//go2 = new GameObject();
	//go2->SetName("2");
	//go2->SetParent(go1);
	//go2->SetLocalPosition(0, 4, 0);
	//go2->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go2->AddComponent<BoxCollider>();


	//GameObject* go3 = new GameObject();
	//go3->SetName("3");
	//go3->SetParent(go2);
	//go3->SetLocalPosition(0, 2, 0);
	//go3->AddComponent<Renderer>(pbrMaterial, sphereMesh);

	//go4 = new GameObject();
	//go4->SetName("4");
	//go4->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go4->AddComponent<BoxCollider>();

	//go5 = new GameObject();
	//go5->SetName("5");
	//go5->AddComponent<Renderer>(greenMaterial, sphereMesh);
	//go5->AddComponent<SphereCollider>();

	//go6 = new GameObject();
	//go6->SetName("6");
	//go6->SetPosition(0, 0, -15);
	//go6->SetLocalRotation(45, 0, 90);
	//go6->SetLocalScale(1, 2, 1);
	//go6->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go6->AddComponent<BoxCollider>();

	//go7 = new GameObject();
	//go7->SetName("7");
	//go7->SetParent(go6);
	//go7->SetLocalPosition(0, 4, 0);
	//go7->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go7->AddComponent<BoxCollider>();

	//GameObject* go8 = new GameObject();
	//go8->SetName("8");
	//go8->SetParent(go7);
	//go8->SetLocalPosition(0, 2, 0);
	//go8->AddComponent<Renderer>(pbrMaterial, sphereMesh);

	//go9 = new GameObject();
	//go9->SetName("9");
	//go9->AddComponent<Renderer>(greenMaterial, cubeMesh);
	//go9->AddComponent<BoxCollider>();

	//go10 = new GameObject();
	//go10->SetName("10");
	//go10->AddComponent<Renderer>(greenMaterial, sphereMesh);
	//go10->AddComponent<SphereCollider>();


	//GameObject* ground = new GameObject();
	//ground->SetLocalPosition(0, -0.5f, 0);
	//ground->SetLocalScale(20, 1, 20);
	//ground->AddComponent<Renderer>(pbrMaterial, cubeMesh);
	for (float i = 0; i < 50; i += 15)
		for (float j = 0; j < 50; j += 15)
		{
			GameObject* ground = new GameObject();
			ground->SetLocalPosition(-18 + i, -0.5f, -18 + j);
			ground->SetLocalScale(15.01f, 1, 15);
			ground->AddComponent<Renderer>(pbrMaterial, cubeMesh);
		}

	for (int i = 0; i < 128; ++i)
	{
		GameObject* light = new GameObject();
		light->SetLocalPosition(rand() % 61 - 30, 2, rand() % 61 - 30);

		switch (rand() % 3)
		{
			case 0:
				light->AddComponent<PointLight>(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), 10, 5);
				break;

			case 1:
				light->AddComponent<PointLight>(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), 10, 5);
				break;

			case 2:
				light->AddComponent<PointLight>(XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), 10, 5);
				break;
		}
		
		light->SetActive(false);


		lights[i] = light;
	}

	lightCount = 50;
	
	//GameObject* ground = new GameObject();
	//ground->SetLocalPosition(0, -0.5f, 0);
	//ground->SetLocalScale(1, 1, 1);
	//ground->AddComponent<Renderer>(pbrMaterial, cubeMesh);

	/*GameObject* test = new GameObject();
	test->SetLocalPosition(0, 1.0f, 1);
	test->AddComponent<Renderer>(pbrMaterial, cubeMesh);*/


	GameObject* trainingPole = new GameObject();
	trainingPole->SetLocalPosition(0, 2, 10);
	trainingPole->SetLocalScale(1, 4, 1);
	trainingPole->AddComponent<Renderer>(greenMaterial, cubeMesh);
	trainingPole->AddComponent<BoxCollider>();
	trainingPole->AddComponent<HitReaction>(redMaterial, greenMaterial);

	GameObject* rightHandObject = new GameObject();
	rightHandRenderer = rightHandObject->AddComponent<Renderer>(greenMaterial, sphereMesh);
	rightHandCollider = rightHandObject->AddComponent<SphereCollider>(0.15f);
	rightHandObject->SetLocalPosition(-115, 167, 4);
	rightHandObject->SetLocalScale(30);
	rightHandRenderer->SetActive(false);

	GameObject* leftHandObject = new GameObject();
	leftHandRenderer = leftHandObject->AddComponent<Renderer>(greenMaterial, sphereMesh);
	leftHandCollider = leftHandObject->AddComponent<SphereCollider>(0.15f);
	leftHandObject->SetLocalPosition(115, 167, 0);
	leftHandObject->SetLocalScale(30);
	leftHandRenderer->SetActive(false);


	model->anim->BindToSlot(23, rightHandObject);
	model->anim->BindToSlot(42, leftHandObject);

	mobModel->anim->SetAnimationIndex(1, true);


	player = new GameObject();
	player->SetLocalPosition(0, 0, -100);
	player->SetLocalRotation(0, 180, 0);
	player->SetLocalScale(0.01f);
	player->AddComponent<Renderer>(skeletonMaterial, model);


	cameraHolder = new GameObject();
	cameraHolder->SetParent(player);
	cameraHolder->SetLocalPosition(0, 0, 0);

	camera = new GameObject();
	camera->SetParent(cameraHolder);
	camera->SetLocalPosition(0, 3, -5.5f);
	camera->SetLocalRotation(15, 0, 0);
	camera->AddComponent<Camera>();


	mob = new GameObject();
	mob->SetLocalPosition(4, 0, -5);
	mob->SetLocalRotation(0, 180, 0);
	mob->SetLocalScale(0.01f);
	mob->AddComponent<Renderer>(mobMaterial, mobModel);
	mob->AddComponent<BoxCollider>(100, 400, 100);
	mob->AddComponent<Mob>(mobModel);
}

void ScratchEngine::Game::OnResize()
{
	DXCore::OnResize();

	Global::SetScreenRatio((float)width / height);
}

void ScratchEngine::Game::UpdateRenderables()
{
	scene->UpdateRenderables();
	scene->SortRenderables();
}

void ScratchEngine::Game::UpdateLightSources()
{
	scene->UpdateLightSources();
}

void ScratchEngine::Game::UpdateViewers()
{
	scene->UpdateViewers();
}

void ScratchEngine::Game::UpdateScene()
{
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	float speed = 1.5f;
	bool animationChanged = false;

	if (Input::IsKeyPressed('1'))
	{
		renderingMode = 1;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('2'))
	{
		renderingMode = 2;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('3'))
	{
		renderingMode = 3;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('4'))
	{
		renderingMode = 4;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('5'))
	{
		renderingMode = 5;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('6'))
	{
		renderingMode = 6;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('7'))
	{
		renderingMode = 2;
		directionalLight->SetActive(false);
		ambientLight->SetActive(false);
	}
	else if (Input::IsKeyPressed('8'))
	{
		renderingMode = 7;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('9'))
	{
		renderingMode = 9;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}
	else if (Input::IsKeyPressed('0'))
	{
		renderingMode = 0;
		directionalLight->SetActive(true);
		ambientLight->SetActive(true);
	}


	if (Input::IsKeyPressed('V'))
	{
		rightHandRenderer->SetActive(!rightHandRenderer->IsActiveSelf());
		leftHandRenderer->SetActive(!leftHandRenderer->IsActiveSelf());
	}

	if ((GetKeyState('H') & 0x8000) != 0 && lastInputTime < totalTime)
	{
		//useBlending
		model->anim->useBlending = false;
		printf("Blending OFF\n");
		lastInputTime = totalTime + 0.1f;
	}

	if ((GetKeyState('G') & 0x8000) != 0 && lastInputTime < totalTime)
	{
		//useBlending
		model->anim->useBlending = true;
		printf("Blending ON\n");
		lastInputTime = totalTime + 0.1f;
	}


	if ((GetKeyState('F') & 0x8000) != 0)
	{
		//attacking
		if (!animationChanged && lastAttackTime < totalTime)
		{
			float duration = model->anim->SetAnimationIndex(combo[comboCounter], false);
			attacking = true;
			animationChanged = true;
			lastInputTime = totalTime + duration;
			lastAttackTime = totalTime + duration - 0.2f;
			comboCounter++;
			if (comboCounter > 4) {
				lastAttackTime = totalTime + duration + 0.5f;
			}
			comboCounter %= 5;
		}
	}


	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !animationChanged&& lastAttackTime < totalTime)
	{
		float duration = model->anim->SetAnimationIndex(2, false);
		lastInputTime = totalTime + model->anim->duration;
		attacking = true;
		animationChanged = true;
		lastInputTime = totalTime + duration;
		lastAttackTime = totalTime + duration - 0.2f;
	}

	if (attacking && lastInputTime < totalTime)
	{
		// the attacking interval is gone
		comboCounter = 0;
		attacking = false;
	}


	if ((GetAsyncKeyState('A') & 0x8000 && GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState('W') & 0x8000 && GetAsyncKeyState('S') & 0x8000))
		animationChanged = true;


	if (!attacking)
	{
		if (GetAsyncKeyState('A') & 0x8000 && !GetAsyncKeyState(VK_LSHIFT)) {
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(6, true);
				animationChanged = true;
			}
			player->Translate(speed * deltaTime, 0, 0);
			lastInputTime = totalTime;
		}
		else if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState('A') & 0x8000) {
			// left sprint
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(10, true);
				animationChanged = true;
			}
			player->Translate(speed * deltaTime * 2.0f, 0, 0);
			lastInputTime = totalTime;
		}

		if (GetAsyncKeyState('D') & 0x8000 && !GetAsyncKeyState(VK_LSHIFT)) {

			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(7, true);
				animationChanged = true;
			}
			player->Translate(-speed * deltaTime, 0, 0);
			lastInputTime = totalTime;
		}
		else if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState('D') & 0x8000) {
			// right sprint
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(11, true);
				animationChanged = true;
			}
			player->Translate(-speed * deltaTime * 2.0f, 0, 0);
			lastInputTime = totalTime;
		}

		if (GetAsyncKeyState('W') & 0x8000 && !GetAsyncKeyState(VK_LSHIFT)) {
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(4, true);
				animationChanged = true;
			}

			player->Translate(0, 0, -speed * deltaTime * 1.0f);
			lastInputTime = totalTime;
		}
		else if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState('W') & 0x8000) {
			// forward sprint
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(8, true);
				animationChanged = true;
			}
			player->Translate(0, 0, -speed * deltaTime * 2.0f);
			lastInputTime = totalTime;
		}


		//camera->Translate(-10 * deltaTime, 0.0f, 0.0f);

		if (GetAsyncKeyState('S') & 0x8000 && !GetAsyncKeyState(VK_LSHIFT)) {

			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(5, true);
				animationChanged = true;
			}
			player->Translate(0, 0, speed * deltaTime);
			lastInputTime = totalTime;
		}
		else if (GetAsyncKeyState(VK_LSHIFT) && GetAsyncKeyState('S') & 0x8000) {
			// forward sprint
			if (!animationChanged && lastInputTime < totalTime) {
				model->anim->SetAnimationIndex(9, true);
				animationChanged = true;
			}
			player->Translate(0, 0, speed * deltaTime * 2.0f);
			lastInputTime = totalTime;
		}

		//camera->Translate(0.0f, 0.0f, -10 * deltaTime);


		if (lastInputTime < totalTime - 5) {
			int idleIndex = rand() % 2 + 12;
			model->anim->SetAnimationIndex(idleIndex, false);
			lastInputTime = totalTime + model->anim->duration;
		}
		else if (lastInputTime < totalTime - 0.2f) {
			model->anim->SetAnimationIndex(1, true);
		}
	}


	rightHandCollider->SetActive(attacking);
	leftHandCollider->SetActive(attacking);


	if (mob->GetComponent<Mob>()->hit)
	{
		//get hit 
		mob->GetComponent<Mob>()->hit = false;
		lastTriggerTime = totalTime + mob->GetComponent<Mob>()->duration;
	}
	else if (lastTriggerTime < totalTime - 3)
	{
		int idleIndex = rand() % 2 + 2;
		float duration = mobModel->anim->SetAnimationIndex(idleIndex, true);
		lastTriggerTime = totalTime + duration;
	}
	else if (lastTriggerTime < totalTime - 0.1f)
		mobModel->anim->SetAnimationIndex(1, true);


	// Handle light count changes, clamped appropriately
	if (Input::IsKeyPressed(VK_UP))
		++lightCount;

	if (Input::IsKeyPressed(VK_DOWN))
		--lightCount;

	lightCount = __max(0, __min(128, lightCount));


	// Move lights
	for (int i = 0; i < 128; ++i)
		lights[i]->SetActive(i < lightCount);


	//if (GetAsyncKeyState('X') & 0x8000)
		//camera->Translate(0.0f, -10 * deltaTime, 0.0f);

	model->anim->Update(deltaTime, player);
	mobModel->anim->Update(deltaTime, mob);
	//go1->Rotate(0, 0, 20 * deltaTime);
	//go2->Rotate(0, 0, -50 * deltaTime);
	//go4->SetLocalPosition(0, 5 * sin(totalTime), 15);
	//go5->SetLocalPosition(5 * cos(totalTime), 0, 15);

	//go6->Rotate(0, 0, 20 * deltaTime);
	//go7->Rotate(0, 0, -50 * deltaTime);
	//go9->SetLocalPosition(0, 5 * sin(totalTime), -15);
	//go10->SetLocalPosition(5 * cos(totalTime), 0, -15);

	PhysicsEngine* physicsEngine = PhysicsEngine::GetSingleton();

	physicsEngine->UpdateBoundingVolumes();
	physicsEngine->SolveCollisions();
}

void ScratchEngine::Game::DrawFrame()
{
	if (scene->renderableAllocator.GetNumAllocated() == 0)
		return;


	RenderingEngine* renderingEngine = Rendering::RenderingEngine::GetSingleton();


	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearRenderTargetView(GBufferAlbedoRTV, color);
	context->ClearRenderTargetView(GBufferNormalsRTV, color);
	context->ClearRenderTargetView(GBufferDepthRTV, color); // Not an actual depth buffer!
	context->ClearRenderTargetView(GBufferMaterialRTV, color);
	context->ClearRenderTargetView(DeferredLightBufferRTV, color);


	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);


	renderingEngine->PerformZPrepass(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated());


	switch (renderingMode)
	{
	case 0:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);


		__RenderShadows(renderingEngine, scene, &(scene->viewerAllocator[0]), &viewport);


		context->RSSetViewports(1, &viewport);
		context->RSSetState(0);


		ID3D11ShaderResourceView* gBufferSRVs[4] = { GBufferAlbedoSRV, GBufferNormalsSRV, GBufferDepthSRV, GBufferMaterialSRV };
		renderingEngine->DrawCSMIndices(&(scene->viewerAllocator[0]), scene->lightSourceAllocator, scene->lightSourceAllocator.GetNumAllocated(), gBufferSRVs, DeferredLightBufferRTV, depthStencilView, stencilSRV);


		renderingEngine->DrawDeferred(DeferredLightBufferSRV, backBufferRTV, depthStencilView);
	}
	break;


	case 1:
		context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
		renderingEngine->DrawForward(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), scene->lightSourceAllocator, scene->lightSourceAllocator.GetNumAllocated());
		break;


	case 2:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);


		__RenderShadows(renderingEngine, scene, &(scene->viewerAllocator[0]), &viewport);


		context->RSSetViewports(1, &viewport);
		context->RSSetState(0);


		ID3D11ShaderResourceView* gBufferSRVs[4] = { GBufferAlbedoSRV, GBufferNormalsSRV, GBufferDepthSRV, GBufferMaterialSRV };
		renderingEngine->DrawLightBuffer(&(scene->viewerAllocator[0]), scene->lightSourceAllocator, scene->lightSourceAllocator.GetNumAllocated(), gBufferSRVs, DeferredLightBufferRTV, depthStencilView, stencilSRV);


		renderingEngine->DrawDeferred(DeferredLightBufferSRV, backBufferRTV, depthStencilView);
	}
	break;


	case 3:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { backBufferRTV, GBufferNormalsRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);
	}
	break;


	case 4:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, backBufferRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);
	}
	break;


	case 5:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, backBufferRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);
	}
	break;


	case 6:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, GBufferDepthRTV, backBufferRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);
	}
	break;


	case 7:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);


		renderingEngine->RenderSSAO(backBufferRTV, &scene->viewerAllocator[0].projectionMatrix, depthSRV, GBufferNormalsSRV);
	}
	break;


	case 9:
	{
		ID3D11RenderTargetView* gBufferRTVs[4] = { GBufferAlbedoRTV, GBufferNormalsRTV, GBufferDepthRTV, GBufferMaterialRTV };
		renderingEngine->DrawGBuffers(&(scene->viewerAllocator[0]), scene->renderableAllocator, scene->renderableAllocator.GetNumAllocated(), gBufferRTVs, 4, depthStencilView);


		__RenderShadows(renderingEngine, scene, &(scene->viewerAllocator[0]), &viewport);


		context->RSSetViewports(1, &viewport);
		context->RSSetState(0);


		ID3D11ShaderResourceView* gBufferSRVs[4] = { GBufferAlbedoSRV, GBufferNormalsSRV, GBufferDepthSRV, GBufferMaterialSRV };
		renderingEngine->DrawLightBuffer(&(scene->viewerAllocator[0]), scene->lightSourceAllocator, scene->lightSourceAllocator.GetNumAllocated(), gBufferSRVs, DeferredLightBufferRTV, depthStencilView, depthSRV);


		renderingEngine->DrawDeferred(DeferredLightBufferSRV, backBufferRTV, depthStencilView);
	}
	break;
	}


	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);

	renderingEngine->RenderSkybox(sky, &(scene->viewerAllocator[0]));


	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->RSSetState(0);

	// Reset depth state
	context->OMSetDepthStencilState(0, 0);


	swapChain->Present(0, 0);


	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);
}

void ScratchEngine::Game::Update()
{
	while (isRunning)
	{
		UpdateTimer();

		if (titleBarStats)
			UpdateTitleBarStats();


		InputManager::singleton->Capture();


		frameBarrier.Wait();


		UpdateScene();


		frameBarrier.Wait();
	}


	allThreadBarrier.Wait();
}

void ScratchEngine::Game::Draw()
{
	while (isRunning)
	{
		Job* job_updateRenderables = jobSystem.Execute([&]() { this->UpdateRenderables(); }, 0);
		Job* job_updateLightSources = jobSystem.Execute([&]() { this->UpdateLightSources(); }, 1);

		UpdateViewers();


		job_updateRenderables->Wait();
		job_updateLightSources->Wait();

		frameBarrier.Wait();


		DrawFrame();


		frameBarrier.Wait();
	}


	allThreadBarrier.Wait();
}

void ScratchEngine::Game::Stop()
{
	jobSystem.DeactivateAllThreads();
}

#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0002)
	{
		camX = (y - prevMousePos.y) * 0.05f;
		camY = (x - prevMousePos.x) * 0.05f;
		cameraHolder->SetLocalRotation(0, 180, 0);
		player->Rotate(0, camY, 0.0f);
		if (model->anim->currentAnimationIndex == 1 ||
			model->anim->currentAnimationIndex == 12 ||
			model->anim->currentAnimationIndex == 13
			) {
			model->anim->SetAnimationIndex(3, false);
			lastInputTime = totalTime + model->anim->duration;
		}
	}
	else if (buttonState & 0x0001) {
		camX = (y - prevMousePos.y) * 0.05f;
		camY = (x - prevMousePos.x) * 0.05f;
		cameraHolder->Rotate(0, camY, 0.0f);
	}

	// the code for rotating camera, not done yet
	//else{
	//	camX = (y - prevMousePos.y) * 0.1f;
	//	camY = (x - prevMousePos.x) * 0.1f;
	//	cameraHolder->Rotate(0, camY, 0.0f);
	//	//SetCursorPos(3440/2, 1440/2);
	//}


	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void ScratchEngine::Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
	if(camera->GetLocalPosition().m128_f32[2] + wheelDelta < -0.5f &&
		camera->GetLocalPosition().m128_f32[2] + wheelDelta > -12)
		camera->Translate(0, 0, wheelDelta * 0.3f);
}
#pragma endregion

