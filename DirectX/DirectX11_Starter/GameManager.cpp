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
	gameObjects.clear();
	menuObjects.clear();
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

	delete camera;
	delete spriteBatch;
	delete spriteFont24;
	delete spriteFont32;
	delete spriteFont72;

	// Release DX
	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(inputLayout);
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(blendState);
	ReleaseMacro(linearSampler);
	ReleaseMacro(anisotropicSampler);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{
	if (!DirectXGame::Init())
		return false;

	// Create samplers
	CreateSamplers();

	// Load shaders that we want
	LoadShadersAndInputLayout();

	LoadMeshesAndMaterials();

	// Set up block types
	BuildBlockTypes();

	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont24 = new SpriteFont(device, L"jing24.spritefont");
	spriteFont32 = new SpriteFont(device, L"jing32.spritefont");
	spriteFont72 = new SpriteFont(device, L"jing72.spritefont");

	// Load the frame
	gameObjects.emplace_back(new GameObject(frameMesh, frameMaterial, &XMFLOAT3(-3.0f, -5.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));
	gameObjects.emplace_back(new GameObject(environmentMesh, tileMaterial, &XMFLOAT3(-50.0f, -5.0f, -75.0f), &XMFLOAT3(0, 0, 0)));

	// Set up block manager
	for (int j = 0; j < GRID_HEIGHT; j++) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			cubes.push_back(new GameObject(cubeMesh, shapeMaterial, &XMFLOAT3((float)i - 4.5f, (float)j - 5.0f, 0), &XMFLOAT3(0, 0, 0)));
		}
	}
	blockManager = new BlockManager(blocks, 7, cubes, XMFLOAT3(-4.5, -5, 0), XMFLOAT3(-8.5, 12.5, 0), 1);
	blockManager->spawnFallingBlock();

	// Create 2D meshes
	//triangleMesh = new Mesh(device, deviceContext, TRIANGLE);
	quadMesh = new Mesh(device, deviceContext, QUAD);
	playButton = new Button(quadMesh, buttonMaterial, &XMFLOAT3(200, 250, 0), spriteBatch, spriteFont32, L"Play");
	quitButton = new Button(quadMesh, buttonMaterial, &XMFLOAT3(200, 400, 0), spriteBatch, spriteFont32, L"Quit");
	scoreLabel = new UIObject(quadMesh, labelMaterial, &XMFLOAT3(0, 0, 0), spriteBatch, spriteFont24, L"Score:\n0");
	menuObjects.emplace_back(new UIObject(quadMesh, titleMaterial, &XMFLOAT3(100, 50, 0), spriteBatch, spriteFont72, L"Tetris"));
	menuObjects.emplace_back(playButton);
	menuObjects.emplace_back(quitButton);
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
	D3D11_SAMPLER_DESC* linearSampleState = new D3D11_SAMPLER_DESC();
	linearSampleState->Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearSampleState->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSampleState->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSampleState->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	device->CreateSamplerState(linearSampleState, &linearSampler);
	delete linearSampleState;

	// Sample state - anisotropic wrap filtering
	D3D11_SAMPLER_DESC* anisotropicState = new D3D11_SAMPLER_DESC();
	anisotropicState->Filter = D3D11_FILTER_ANISOTROPIC;
	anisotropicState->AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicState->AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicState->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicState->MaxAnisotropy = 1;
	device->CreateSamplerState(anisotropicState, &anisotropicSampler);
	delete anisotropicState;
	anisotropicSampler = linearSampler;
}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void GameManager::LoadShadersAndInputLayout()
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Set up the geometry layout description
	// [Particle System]
	D3D11_SO_DECLARATION_ENTRY soDesc[] =
	{
		// stream number, semantic name, semantic index, start component, component count, output slot
		{ 0, "SV_POSITION", 0, 0, 4, 0 },   // output all components of position
		{ 0, "NORMAL", 0, 0, 3, 0 },     // output the first 3 of the normal
		{ 0, "TEXCOORD0", 0, 0, 2, 0 },     // output the first 2 texture coordinates
	};

	// Load Vertex Shader --------------------------------------
	ID3DBlob* vsBlob;
	D3DReadFileToBlob(L"VertexShader.cso", &vsBlob);

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&vertexShader));

	// Before cleaning up the data, create the input layout
	HR(device->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&inputLayout));

	// Clean up
	ReleaseMacro(vsBlob);

	// Load Geometry Shader -------------------------------------
	// [Particle System]
	LoadGeometryShader(L"GeometryShader.cso", &geometryShader);

	// Load Pixel Shaders ---------------------------------------
	LoadPixelShader(L"PixelShader.cso", &pixelShader);
	LoadPixelShader(L"GrayscalePixelShader.cso", &grayscaleShader);
	LoadPixelShader(L"SepiaPixelShader.cso", &sepiaShader);
	LoadPixelShader(L"InversePixelShader.cso", &inverseShader);

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

	// Set the Ouput Targets -----------------------------------
	// [Particle System]
	ID3D11Buffer* soBuffer;
	int bufferSize = 1000000;
	D3D11_BUFFER_DESC soBufferDesc =
	{
		bufferSize,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_STREAM_OUTPUT,	// Flag indicates buffer resource to be updated frequently by CPU
		0,
		0,
		0
	};
	HR(device->CreateBuffer(
		&soBufferDesc,
		NULL,
		&soBuffer));
	// Set stream buffer to current device
	UINT offset[1] = { 0 };
	deviceContext->SOSetTargets(1, &soBuffer, offset);	// num buffers, pointer to buffers, array of offsets
}

void GameManager::LoadPixelShader(wchar_t* file, ID3D11PixelShader** shader) {
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

void GameManager::LoadGeometryShader(wchar_t* file, ID3D11GeometryShader** shader)
{
	ID3DBlob* gsBlob;
	D3DReadFileToBlob(L"GeometryShader.cso", &gsBlob);

	// Create the shader on the device
	HR(device->CreateGeometryShader(
		gsBlob->GetBufferPointer(),
		gsBlob->GetBufferSize(),
		NULL,
		&geometryShader));
	// Result: S_OK
	// Original: D3D11Device->CreateGeometryShaderWithStreamOut( pShaderBytecode, ShaderBytecodesize, pDecl, sizeof(pDecl), NULL, 0, 0, NULL, &pStreamOutGS );

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

#pragma endregion

#pragma region Game Loop

// Updates the buffers (and pushes them to the buffer on the device)
// and draws for each gameObject
void GameManager::UpdateScene(float dt)
{
	CheckKeyBoard(dt);

	// Clear the buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// [DRAW] Set up the input assembler for objects
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Update the camera
	camera->Update(dt);

	// [UPDATE] Update constant buffer data
	dataToSendToVSConstantBuffer.view = camera->viewMatrix;
	dataToSendToVSConstantBuffer.lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.75f);
	dataToSendToVSConstantBuffer.color = XMFLOAT4(1, 1, 1, 1);
	//dataToSendToVSConstantBuffer.resolution = XMFLOAT2(windowWidth, windowHeight);

	// Projection matrix
	dataToSendToVSConstantBuffer.projection = projectionMatrix;

	std::vector<GameObject*> *meshObjects = 0;
	if (gameState == GAME || gameState == DEBUG) meshObjects = &gameObjects;

	std::vector<UIObject*> *uiObjects = 0;
	if (gameState == MENU) uiObjects = &menuObjects;
	if (gameState == GAME || gameState == DEBUG) uiObjects = &gameUIObjects;

	// Update each mesh
	if (meshObjects) {

		for (UINT i = 0; i < meshObjects->size(); i++)
		{
			// [UPDATE] Update this object
			if (gameState != DEBUG)
				(*meshObjects)[i]->Update(dt);

			// [DRAW] Draw the object
			(*meshObjects)[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
		}
	}

	// Update and draw the game if in game mode
	if (gameState == GAME || gameState == DEBUG)
	{
		if (gameState != DEBUG)
			blockManager->update(dt);
		blockManager->draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
	}
	int score = blockManager->getScore();
	std::wstring s = std::wstring(L"Score\n") + std::to_wstring(score);
	const wchar_t* result = s.c_str();
	scoreLabel->SetText(result);

	// Draw UI Elements
	if (uiObjects) {
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

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

// NOTE: DEPRECATED
// Clear the screen, redraw everything, present
void GameManager::DrawScene()
{
	//// Clear the buffer
	//deviceContext->ClearRenderTargetView(renderTargetView, color);
	//deviceContext->ClearDepthStencilView(
	//	depthStencilView,
	//	D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	//	1.0f,
	//	0);

	//// Draw each entity
	//for (UINT i = 0; i < entities.size(); i++)
	//{
	//	// Set up the input assembler
	//	deviceContext->IASetInputLayout(inputLayout);
	//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	deviceContext->VSSetConstantBuffers(
	//		0,	// Corresponds to the constant buffer's register in the vertex shader
	//		1,
	//		&(vsConstantBuffer));

	//	entities[i]->Draw();
	//}

	//// Present the buffer
	//HR(swapChain->Present(0, 0));
}

#pragma endregion

#pragma region User Input

// Continuous while key pressed
void GameManager::CheckKeyBoard(float dt)
{
	// Game controls
	if (gameState == GAME)
	{
		// Move left
		if (GetAsyncKeyState('A'))
		{
			blockManager->move(LEFT);
		}

		// Move Right
		if (GetAsyncKeyState('D'))
		{
			blockManager->move(RIGHT);
		}

		// Move down faster
		if (GetAsyncKeyState('S'))
		{
			blockManager->fallSpeed = FAST_FALL_SPEED;
		}
		else
		{
			blockManager->fallSpeed = SLOW_FALL_SPEED;
		}

		// Rotation
		if (GetAsyncKeyState('W'))
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
			// Change state
			/*
			case VK_SPACE:
			gameState = (gameState == MENU) ? GAME : MENU;

			if (gameState == MENU)
			allObjects = menuObjects;
			else if (gameState == GAME)
			allObjects = gameObjects;
			break;
			*/

			// Movement of game object
		case VK_NUMPAD8:
			//allObjects[0]->Move(&XMFLOAT3(0.0f, 0.2f, 0.0f));
			break;
		case VK_NUMPAD5:
			//allObjects[0]->Move(&XMFLOAT3(0.0f, -0.2f, 0.0f));
			break;
		case VK_NUMPAD6:
			//allObjects[0]->Move(&XMFLOAT3(0.2f, 0.0f, 0.0f));
			break;
		case VK_NUMPAD4:
			//allObjects[0]->Move(&XMFLOAT3(-0.2f, 0.0f, 0.0f));
			break;
			// Rotation of game object
		case VK_NUMPAD7:
			//allObjects[0]->Rotate(&XMFLOAT3(0.0f, 0.0f, XM_PI / 2));
			break;
		case VK_NUMPAD9:
			//allObjects[0]->Rotate(&XMFLOAT3(0.0f, 0.0f, -XM_PI / 2));
			break;
		case VK_CAPITAL:
			gameState = (gameState != DEBUG) ? DEBUG : GAME;
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
			gameState = GAME;
		}
		if (quitButton->IsOver(x, y)) {
			PostQuitMessage(0);
		}
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

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);

	// TODO fix the fact that there is an if statement needed here
	//if (camera)
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
}
#pragma endregion