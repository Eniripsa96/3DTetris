// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the compiled shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include <Windows.h>
#include <d3dcompiler.h>
#include "GameManager.h"
#include <vector>

// Background color
const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Make the game, initialize and run
	GameManager game(hInstance);

	if (!game.Init())
		return 0;

	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

GameManager::GameManager(HINSTANCE hInstance) : DirectXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;

	gameState = MENU;
}

// Clean up here
GameManager::~GameManager()
{
	// Clean up objects
	for (UINT i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	for (UINT i = 0; i < menuObjects.size(); i++)
	{
		delete menuObjects[i];
	}
	for (UINT i = 0; i < cubes.size(); i++)
	{
		delete cubes[i];
	}
	for (UINT i = 0; i < gameUIObjects.size(); i++)
	{
		delete gameUIObjects[i];
	}
	for (UINT i = 0; i < gameOverObjects.size(); i++)
	{
		delete gameOverObjects[i];
	}
	gameObjects.clear();
	menuObjects.clear();
	gameOverObjects.clear();
	gameUIObjects.clear();
	cubes.clear();

	// Clean up blocks
	for (UINT i = 0; i < 7; i++)
	{
		delete blocks[i].gameObject;
		delete[] blocks[i].localGrid;
		delete[] blocks[i].tempGrid;
		delete[] blocks[i].grid;
	}
	delete[] blocks;
	delete blockManager;

	delete[] pixelShaders;

	// Clean up meshes
	delete triangleMesh;
	delete quadMesh;
	delete cubeMesh;
	delete jBlockMesh;
	delete lBlockMesh;
	delete leftBlockMesh;
	delete longBlockMesh;
	delete rightBlockMesh;
	delete squareBlockMesh;
	delete stairsBlockMesh;
	delete frameMesh;
	delete environmentMesh;
	delete particleMesh;

	// Clean up materials
	delete shapeMaterial;
	delete buttonMaterial;
	delete titleMaterial;
	delete labelMaterial;
	delete jBlockMaterial;
	delete lBlockMaterial;
	delete leftBlockMaterial;
	delete longBlockMaterial;
	delete rightBlockMaterial;
	delete squareBlockMaterial;
	delete stairsBlockMaterial;
	delete frameMaterial;
	delete tileMaterial;
	delete particleMaterial;

	delete camera;
	delete particleSystem;
	
	delete spriteBatch;
	delete spriteFont24;
	delete spriteFont32;
	delete spriteFont72;

	// Release DX
	ReleaseMacro(vertexShader);
	ReleaseMacro(shadowVS);
	ReleaseMacro(shadowPS);
	ReleaseMacro(particleVertexShader);
	ReleaseMacro(particleGeometryShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(grayscaleShader);
	ReleaseMacro(sepiaShader);
	ReleaseMacro(inverseShader);

	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(blendState);

	ReleaseMacro(linearSampler);
	ReleaseMacro(anisotropicSampler);
	ReleaseMacro(pointSampler);

	ReleaseMacro(InputLayouts::Vertex);
	ReleaseMacro(InputLayouts::Particle);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{
	if (!DirectXGame::Init())
		return false;

	CreateSamplers();
	LoadShadersAndInputLayout();
	LoadMeshesAndMaterials();
	BuildBlockTypes();
	//CreateShadowMapResources();

	activeShader = 0;
	pixelShaders = new ID3D11PixelShader*[shaderCount] {
		pixelShader,
		grayscaleShader,
		sepiaShader,
		inverseShader
	};

	Camera* shadowCam = new Camera();
	shadowCam->RotateY(-1.10715f);
	shadowCam->Pitch(-0.930264f);
	shadowCam->MoveTo(&XMFLOAT3(-20, 30, -10));
	shadowCam->Update(0);
	shadowView = shadowCam->viewMatrix;
	delete shadowCam;

	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont24 = new SpriteFont(device, L"jing24.spritefont");
	spriteFont32 = new SpriteFont(device, L"jing32.spritefont");
	spriteFont72 = new SpriteFont(device, L"jing72.spritefont");

	// Load the frame
	gameObjects.emplace_back(new GameObject(frameMesh, frameMaterial, &XMFLOAT3(-3.0f, -5.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));
	gameObjects.emplace_back(new GameObject(environmentMesh, tileMaterial, &XMFLOAT3(-50.0f, -5.0f, -75.0f), &XMFLOAT3(0, 0, 0)));

	// Create particle system
	particleSystem = new ParticleSystem(particleMesh, particleMaterial);

	// Set up block manager
	for (int j = 0; j < GRID_HEIGHT; j++) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			cubes.push_back(new GameObject(cubeMesh, shapeMaterial, &XMFLOAT3((float)i - 4.5f, (float)j - 5.0f, 0), &XMFLOAT3(0, 0, 0)));
		}
	}
	blockManager = new BlockManager(blocks, 7, cubes, XMFLOAT3(-4.5, -5, 0), XMFLOAT3(-8.5, 12.5, 0), 1, particleSystem);
	blockManager->spawnFallingBlock();

	// Create 2D meshes
	//triangleMesh = new Mesh(device, deviceContext, TRIANGLE);
	quadMesh = new Mesh(device, deviceContext, QUAD);
	playButton = new Button(quadMesh, buttonMaterial, &XMFLOAT3(200, 250, 0), spriteBatch, spriteFont32, L"Play");
	quitButton = new Button(quadMesh, buttonMaterial, &XMFLOAT3(200, 400, 0), spriteBatch, spriteFont32, L"Quit");
	mainMenuButton = new Button(quadMesh, buttonMaterial, &XMFLOAT3(200, 300, 0), spriteBatch, spriteFont32, L"Main Menu");
	scoreLabel = new UIObject(quadMesh, labelMaterial, &XMFLOAT3(0, 0, 0), spriteBatch, spriteFont24, L"Score:\n0");
	menuObjects.emplace_back(new UIObject(quadMesh, titleMaterial, &XMFLOAT3(100, 50, 0), spriteBatch, spriteFont72, L"Tetris"));
	menuObjects.emplace_back(playButton);
	menuObjects.emplace_back(quitButton);
	gameOverObjects.emplace_back(new UIObject(quadMesh, titleMaterial, &XMFLOAT3(100, 100, 0), spriteBatch, spriteFont72, L"Game Over"));
	gameOverObjects.emplace_back(mainMenuButton);
	gameUIObjects.emplace_back(scoreLabel);

	// Blend state - enabling alpha blending
	BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &blendState);

	camera = new Camera();

	// Set up the world matrix for each mesh
	XMMATRIX W = XMMatrixIdentity();
	for (UINT i = 0; i < gameObjects.size(); i++)
		XMStoreFloat4x4(&(gameObjects[i]->worldMatrix), XMMatrixTranspose(W));

	return true;
}

// Creates the samplers used by the game
void GameManager::CreateSamplers() {

	// Sample state - linear wrap filtering
	D3D11_SAMPLER_DESC* desc = new D3D11_SAMPLER_DESC();
	desc->MaxLOD = D3D11_FLOAT32_MAX;
	desc->Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(desc, &linearSampler);

	// Sample state - anisotropic wrap filtering
	desc->Filter = D3D11_FILTER_ANISOTROPIC;
	desc->MaxAnisotropy = 16;
	device->CreateSamplerState(desc, &anisotropicSampler);
	
	// Sample state - point wrap filtering
	desc->Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc->AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc->AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc->AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(desc, &pointSampler);
	delete desc;
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void GameManager::LoadShadersAndInputLayout()
{
	// Load Vertex Shaders --------------------------------------
	LoadVertexShader(L"VertexShader.cso", VERTEX_LAYOUT, &vertexShader);
	LoadVertexShader(L"ShadowVertexShader.cso", SHADOW_LAYOUT, &shadowVS);
	LoadVertexShader(L"ParticleVertexShader.cso", PARTICLE_LAYOUT, &particleVertexShader);

	// Load Geometry Shader -------------------------------------
	LoadGeometryShader(L"ParticleGeometryShader.cso", &particleGeometryShader);

	// Load Pixel Shaders ---------------------------------------
	LoadPixelShader(L"PixelShader.cso", &pixelShader);
	LoadPixelShader(L"GrayscalePixelShader.cso", &grayscaleShader);
	LoadPixelShader(L"SepiaPixelShader.cso", &sepiaShader);
	LoadPixelShader(L"InversePixelShader.cso", &inverseShader);
	LoadPixelShader(L"ShadowPixelShader.cso", &shadowPS);
	LoadPixelShader(L"ParticlePixelShader.cso", &particlePixelShader);

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = sizeof(dataToSendToVSConstantBuffer);
	cBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = 0;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&vsConstantBuffer));

	D3D11_BUFFER_DESC GSCBufferDesc;
	GSCBufferDesc.ByteWidth = sizeof(dataToSendToGSConstantBuffer);
	GSCBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	GSCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	GSCBufferDesc.CPUAccessFlags = 0;
	GSCBufferDesc.MiscFlags = 0;
	GSCBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&GSCBufferDesc,
		NULL,
		&gsConstantBuffer));
}

void GameManager::LoadPixelShader(wchar_t* file, ID3D11PixelShader** shader)
{
	ID3DBlob* psBlob;
	D3DReadFileToBlob(file, &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		shader));

	// Clean up
	ReleaseMacro(psBlob);
}

void GameManager::LoadVertexShader(wchar_t* file, LAYOUT inputLayoutType, ID3D11VertexShader** shader)
{
	// Load shader blob
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(file, &vsBlob);

	if (inputLayoutType == VERTEX_LAYOUT)
		InputLayouts::InitializeVertexLayout(device, vsBlob);
	else if (inputLayoutType == PARTICLE_LAYOUT)
		InputLayouts::InitializeParticleLayout(device, vsBlob);
	else
		InputLayouts::InitializeShadowLayout(device, vsBlob);

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		shader));

	// Clean up
	ReleaseMacro(vsBlob);
}

void GameManager::LoadGeometryShader(wchar_t* file, ID3D11GeometryShader** shader)
{
	ID3DBlob* gsBlob; 
	HR(D3DReadFileToBlob(file, &gsBlob));

	// Create the shader on the device
	HR(device->CreateGeometryShader(
		gsBlob->GetBufferPointer(),
		gsBlob->GetBufferSize(),
		NULL,
		&particleGeometryShader));

	// Clean up
	ReleaseMacro(gsBlob);
}

// Load each of the game's meshes and materials
void GameManager::LoadMeshesAndMaterials()
{
	// Prepare some variables
	ObjLoader loader = ObjLoader();
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int size;

	// Create materials
	shapeMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"image.png");
	buttonMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"button.png");
	titleMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"title.png");
	labelMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"label.png");
	jBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texJBlock.png");
	lBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texLBlock.png");
	leftBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texLeftBlock.png");
	longBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texLongBlock.png");
	rightBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texRightBlock.png");
	squareBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texSquareBlock.png");
	stairsBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texStairsBlock.png");
	frameMaterial = new Material(device, deviceContext, vertexShader, pixelShader, linearSampler, L"texFrame.png");
	tileMaterial = new Material(device, deviceContext, vertexShader, pixelShader, anisotropicSampler, L"tile.png");
	particleMaterial = new Material(device, deviceContext, particleVertexShader, particlePixelShader, linearSampler, L"texLBlock.png", particleGeometryShader);

	// Load meshes
	size = loader.Load("cube.txt", device, &vertexBuffer, &indexBuffer);
	cubeMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("jBlock.txt", device, &vertexBuffer, &indexBuffer);
	jBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("lBlock.txt", device, &vertexBuffer, &indexBuffer);
	lBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("leftBlock.txt", device, &vertexBuffer, &indexBuffer);
	leftBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("longBlock.txt", device, &vertexBuffer, &indexBuffer);
	longBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("rightBlock.txt", device, &vertexBuffer, &indexBuffer);
	rightBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("squareBlock.txt", device, &vertexBuffer, &indexBuffer);
	squareBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("stairsBlock.txt", device, &vertexBuffer, &indexBuffer);
	stairsBlockMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("frame.txt", device, &vertexBuffer, &indexBuffer);
	frameMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	size = loader.Load("environment.txt", device, &vertexBuffer, &indexBuffer);
	environmentMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size);
	particleMesh = new Mesh(device, deviceContext, PARTICLE);
}

// Create the structs of the different block types
void GameManager::BuildBlockTypes()
{
	blocks = new Block[7];

	blocks[0].threeByThree = true;
	blocks[0].gameObject = new GameObject(jBlockMesh, jBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.0f, 1.5f, 0.0f));
	blocks[0].localGrid = new bool[9];
	blocks[0].tempGrid = new bool[9];
	blocks[0].grid = new bool[9] {
		true, true, true,
			true, false, false,
			false, false, false
	};

	blocks[1].threeByThree = true;
	blocks[1].gameObject = new GameObject(lBlockMesh, lBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.0f, 1.5f, 0.0f));
	blocks[1].localGrid = new bool[9];
	blocks[1].tempGrid = new bool[9];
	blocks[1].grid = new bool[9] {
		true, true, true,
			false, false, true,
			false, false, false
	};

	blocks[2].threeByThree = true;
	blocks[2].gameObject = new GameObject(leftBlockMesh, leftBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.0f, 1.5f, 0.0f));
	blocks[2].localGrid = new bool[9];
	blocks[2].tempGrid = new bool[9];
	blocks[2].grid = new bool[9] {
		false, true, true,
			true, true, false,
			false, false, false
	};

	blocks[3].threeByThree = false;
	blocks[3].gameObject = new GameObject(longBlockMesh, longBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 2.0f, 0.0f));
	blocks[3].localGrid = new bool[16];
	blocks[3].tempGrid = new bool[16];
	blocks[3].grid = new bool[16] {
		false, false, false, false,
			true, true, true, true,
			false, false, false, false,
			false, false, false, false
	};

	blocks[4].threeByThree = true;
	blocks[4].gameObject = new GameObject(rightBlockMesh, rightBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.0f, 1.5f, 0.0f));
	blocks[4].localGrid = new bool[9];
	blocks[4].tempGrid = new bool[9];
	blocks[4].grid = new bool[9] {
		true, true, false,
			false, true, true,
			false, false, false
	};

	blocks[5].threeByThree = false;
	blocks[5].gameObject = new GameObject(squareBlockMesh, squareBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 2.0f, 0.0f));
	blocks[5].localGrid = new bool[16];
	blocks[5].tempGrid = new bool[16];
	blocks[5].grid = new bool[16] {
		false, false, false, false,
			false, true, true, false,
			false, true, true, false,
			false, false, false, false
	};

	blocks[6].threeByThree = true;
	blocks[6].gameObject = new GameObject(stairsBlockMesh, stairsBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.0f, 1.5f, 0.0f));
	blocks[6].localGrid = new bool[9];
	blocks[6].tempGrid = new bool[9];
	blocks[6].grid = new bool[9] {
		true, true, true,
			false, true, false,
			false, false, false
	};
}

void GameManager::CreateShadowMapResources() 
{
	ReleaseMacro(shadowTex);
	ReleaseMacro(shadowDSV);
	ReleaseMacro(shadowSRV);

	// Texture
	D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width = 2048;
	//texDesc.Height = 2048;
	texDesc.Width = windowWidth;
	texDesc.Height = windowHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &shadowTex));

	// Depth Stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(shadowTex, &descDSV, &shadowDSV));

	// Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	HR(device->CreateShaderResourceView(shadowTex, &srvDesc, &shadowSRV));
}

#pragma endregion

#pragma region Game Loop

// Updates the buffers (and pushes them to the buffer on the device)
// and draws for each gameObject
void GameManager::UpdateScene(float dt)
{
	CheckKeyBoard(dt);

	// Update the game
	if (gameState == GAME)
	{
		blockManager->update(dt);
		if (blockManager->isGameOver())
		{
			gameState = GAME_OVER;
		}
	}

	// Active mesh list
	std::vector<GameObject*> *meshObjects = 0;
	if (gameState == GAME || gameState == DEBUG) meshObjects = &gameObjects;
	
	// Active UI list
	std::vector<UIObject*> *uiObjects = 0;
	if (gameState == MENU) uiObjects = &menuObjects;
	if (gameState == GAME || gameState == DEBUG) uiObjects = &gameUIObjects;
	if (gameState == GAME_OVER) uiObjects = &gameOverObjects;

	// [DRAW] Set up the input assembler for objects
	deviceContext->IASetInputLayout(InputLayouts::Vertex);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Update each mesh
	if (meshObjects) {
		for (UINT i = 0; i < meshObjects->size(); i++) {
			if (gameState != DEBUG) (*meshObjects)[i]->Update(dt);
		}
	}

	// --------- Shadow Map Generation ------------------------------------------------------------------
	
	// Update shadow camera
	//shadowCam->Update(dt);

	// [UPDATE] Update constant buffer data
	dataToSendToVSConstantBuffer.view = camera->viewMatrix;
	dataToSendToVSConstantBuffer.projection = projectionMatrix;
	dataToSendToVSConstantBuffer.lightView = shadowView;
	dataToSendToVSConstantBuffer.lightProjection = shadowProjection;
	dataToSendToVSConstantBuffer.lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.95f);
	dataToSendToVSConstantBuffer.color = XMFLOAT4(1, 1, 1, 1);

	dataToSendToGSConstantBuffer.view = camera->viewMatrix;
	dataToSendToGSConstantBuffer.projection = projectionMatrix;

	// Shadow map
	deviceContext->OMSetRenderTargets(0, 0, shadowDSV);
	//deviceContext->OMSetRenderTargets(1, &renderTargetView, shadowDSV);
	deviceContext->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->VSSetShader(shadowVS, 0, 0);
	deviceContext->PSSetShader(0, 0, 0);
	//deviceContext->PSSetShader(shadowPS, 0, 0);

	// Draw mesh objects that can cast shadows
	if (meshObjects) {
		for (UINT i = 0; i < meshObjects->size(); i++) {
			(*meshObjects)[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
		}
	}
	// Draw the game if in game mode
	if (gameState == GAME || gameState == DEBUG)
	{
		blockManager->draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
	}

	//return;

	// ----------- Normal Rendering --------------------------------------------------------

	// Update the camera
	camera->Update(dt);

	// Clear the buffer
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Bind shadow map
	deviceContext->PSSetShaderResources(1, 1, &shadowSRV);
	deviceContext->PSSetSamplers(1, 1, &pointSampler);

	// Set the shaders
	deviceContext->VSSetShader(vertexShader, 0, 0);
	deviceContext->PSSetShader(pixelShaders[activeShader], 0, 0);

	//bind shadow map texture
	deviceContext->PSSetShaderResources(1, 1, &shadowSRV);
	deviceContext->PSSetSamplers(1, 1, &pointSampler);
	
	// Draw each mesh
	if (meshObjects) {
		for (UINT i = 0; i < meshObjects->size(); i++) {
			(*meshObjects)[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
		}
	}

	// Draw the game if in game mode
	if (gameState == GAME || gameState == DEBUG)
	{
		blockManager->draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
	}

	// Draw the particle system
	if (gameState == GAME || gameState == DEBUG)
	{
		// [DRAW] Set up the input assembler for particle system
		deviceContext->IASetInputLayout(InputLayouts::Particle);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		particleSystem->GetMaterial()->SetShaders();

		if (gameState != DEBUG)
			// [UPDATE] Update the particle system 
			particleSystem->Update(&dataToSendToGSConstantBuffer, dt);

		// [DRAW] Draw the particle system
		particleSystem->Draw(deviceContext, *camera, gsConstantBuffer, &dataToSendToGSConstantBuffer);

		deviceContext->GSSetShader(NULL, 0, 0);
	}
	
	// Draw UI Elements
	if (uiObjects) {

		int score = blockManager->getScore();
		std::wstring s = std::wstring(L"Score\n") + std::to_wstring(score);
		const wchar_t* result = s.c_str();
		scoreLabel->SetText(result);

		spriteBatch->Begin();
		for (UINT i = 0; i < uiObjects->size(); i++)
		{
			// [DRAW] Draw the object
			(*uiObjects)[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
		}
		spriteBatch->End();

		deviceContext->OMSetBlendState(blendState, NULL, 0xffffffff);
		deviceContext->OMSetDepthStencilState(0, 0);
	}
	
	ID3D11ShaderResourceView* nullView = NULL;
	deviceContext->PSSetShaderResources(1, 1, &nullView);

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

// NOTE: DEPRECATED
// Clear the screen, redraw everything, present
void GameManager::DrawScene() { } 

#pragma endregion

#pragma region User Input

// Continuous while key pressed
void GameManager::CheckKeyBoard(float dt)
{
	// Game controls
	if (gameState == GAME)
	{
		// Move left
		if (GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT))
		{
			blockManager->move(LEFT);
		}

		// Move Right
		if (GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT))
		{
			blockManager->move(RIGHT);
		}

		// Move down faster
		if (GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN))
		{
			blockManager->fallSpeed = FAST_FALL_SPEED;
		}
		else
		{
			blockManager->fallSpeed = SLOW_FALL_SPEED;
		}

		// Rotation
		if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP))
		{
			if (canRotate)
			{
				blockManager->rotate();
				canRotate = false;
			}
		}
		else
		{
			canRotate = true;
		}

		// Holding blocks
		if (GetAsyncKeyState('E'))
		{
			blockManager->holdBlock();
		}
	}
	else if (gameState == DEBUG)
	{
		// Strafing of camera
		if (GetAsyncKeyState('A'))
			camera->MoveHorizontal(-CAMERA_MOVE_FACTOR * dt);
		else if (GetAsyncKeyState('D'))
			camera->MoveHorizontal(CAMERA_MOVE_FACTOR * dt);
		// Forward movement of camera
		if (GetAsyncKeyState('W'))
			camera->MoveDepth(CAMERA_MOVE_FACTOR * dt);
		else if (GetAsyncKeyState('S'))
			camera->MoveDepth(-CAMERA_MOVE_FACTOR * dt);
		if (GetAsyncKeyState('Q'))
			camera->MoveVertical(CAMERA_MOVE_FACTOR * dt);
		else if (GetAsyncKeyState('E'))
			camera->MoveVertical(-CAMERA_MOVE_FACTOR * dt);
	}
}

// Once per key press
LRESULT GameManager::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		// Instant drop
		case VK_SHIFT:
			blockManager->drop();
		// Switch to debug mode
		case VK_CAPITAL:
			gameState = (gameState != DEBUG) ? DEBUG : GAME;
			break;
		// Change the active shader
		case VK_TAB:
			activeShader = (activeShader + 1) % shaderCount;
			break;
		}
	}

	return DirectXGame::MsgProc(hwnd, msg, wParam, lParam);
}

// These methods don't do much currently, but can be used for mouse-related input

void GameManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void GameManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

	// Main menu buttons
	if (gameState == MENU) {
		if (playButton->IsOver(x, y)) {
			blockManager->reset();
			gameState = GAME;
		}
		if (quitButton->IsOver(x, y)) {
			PostQuitMessage(0);
		}
	}
	else if (gameState == GAME_OVER && mainMenuButton->IsOver(x, y)) {
		gameState = MENU;
	}
}

void GameManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (gameState == DEBUG && (btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - prevMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - prevMousePos.y));

		camera->RotateY(-dx);
		camera->Pitch(-dy);
	}

	playButton->Update(x, y);
	quitButton->Update(x, y);
	mainMenuButton->Update(x, y);

	prevMousePos.x = x;
	prevMousePos.y = y;
}


#pragma endregion

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void GameManager::OnResize()
{
	// Handle base-level DX resize stuff
	DirectXGame::OnResize();

	// Shadow map projection
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.5f * 3.1415926535f,
		//1,
		AspectRatio(),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&shadowProjection, XMMatrixTranspose(P));

	// Update our projection matrix since the window size changed
	P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);

	// TODO fix the fact that there is an if statement needed here
	//if (camera)
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));

	CreateShadowMapResources();
}
#pragma endregion