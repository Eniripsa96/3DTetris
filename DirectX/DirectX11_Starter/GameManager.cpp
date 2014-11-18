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
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	GameManager game(hInstance);
	
	if( !game.Init() )
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
	// Clean up game objects
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

	// Clean up blocks
	for (UINT i = 0; i < 7; i++)
	{
		delete blocks[i].gameObject;
		delete[] blocks[i].localGrid;
		delete[] blocks[i].tempGrid;
		delete[] blocks[i].grid;	// I think this is causing our 7 0byte memory leaks
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

	// Clean up materials
	delete shapeMaterial;
	delete jBlockMaterial;
	delete lBlockMaterial;
	delete leftBlockMaterial;
	delete longBlockMaterial;
	delete rightBlockMaterial;
	delete squareBlockMaterial;
	delete stairsBlockMaterial;
	delete frameMaterial;

	delete camera;

	// Release DX
	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(inputLayout);
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool GameManager::Init()
{
	if( !DirectXGame::Init() )
		return false;

	// Load shaders that we want
	LoadShadersAndInputLayout();

	LoadMeshesAndMaterials();

	// Set up block types
	BuildBlockTypes();

	// Load the frame
	gameObjects.emplace_back(new GameObject(frameMesh, frameMaterial, &XMFLOAT3(-3.0f, -5.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));

	// Set up block manager
	for (int j = 0; j < GRID_HEIGHT; j++) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			cubes.push_back(new GameObject(cubeMesh, shapeMaterial, &XMFLOAT3((float)i - 4.5f, (float)j - 5.0f, 0), &XMFLOAT3(0, 0, 0)));
		}
	}
	blockManager = new BlockManager(blocks, 7, cubes, XMFLOAT3(-5, -5, 0), XMFLOAT3(-8.25, 12.5, 0), 1);
	blockManager->spawnFallingBlock();

	// Create the menu objects we want
	menuObjects.emplace_back(new GameObject(cubeMesh, shapeMaterial, &XMFLOAT3(0.0f, -0.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));

	// Start out displaying the objects for the menu
	allObjects = menuObjects;

	camera = new Camera();

	// Set up the world matrix for each mesh
	XMMATRIX W = XMMatrixIdentity();
	for (UINT i = 0; i < gameObjects.size(); i++)
		XMStoreFloat4x4(&(gameObjects[i]->worldMatrix), XMMatrixTranspose(W));

	return true;
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	// Load Pixel Shader ---------------------------------------
	ID3DBlob* psBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&pixelShader));

	// Clean up
	ReleaseMacro(psBlob);

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
	shapeMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"image.png");
	jBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texJBlock.png");
	lBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texLBlock.png");
	leftBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texLeftBlock.png");
	longBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texLongBlock.png");
	rightBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texRightBlock.png");
	squareBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texSquareBlock.png");
	stairsBlockMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texStairsBlock.png");
	frameMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"texFrame.png");

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
}

// Create the structs of the different block types
void GameManager::BuildBlockTypes()
{
	blocks = new Block[7];

	blocks[0].threeByThree = true;
	blocks[0].gameObject = new GameObject(jBlockMesh, jBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[0].localGrid = new bool[9];
	blocks[0].tempGrid = new bool[9];
	blocks[0].grid = new bool[9] {
		true, true, true,
			true, false, false,
			false, false, false
	};

	blocks[1].threeByThree = true;
	blocks[1].gameObject = new GameObject(lBlockMesh, lBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[1].localGrid = new bool[9];
	blocks[1].tempGrid = new bool[9];
	blocks[1].grid = new bool[9] {
		true, true, true,
			false, false, true,
			false, false, false
	};

	blocks[2].threeByThree = true;
	blocks[2].gameObject = new GameObject(leftBlockMesh, leftBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[2].localGrid = new bool[9];
	blocks[2].tempGrid = new bool[9];
	blocks[2].grid = new bool[9] {
		false, true, true,
			true, true, false,
			false, false, false
	};

	blocks[3].threeByThree = false;
	blocks[3].gameObject = new GameObject(longBlockMesh, longBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(2.0f, 2.0f, 0.0f));
	blocks[3].localGrid = new bool[16];
	blocks[3].tempGrid = new bool[16];
	blocks[3].grid = new bool[16] {
		false, false, false, false,
			true, true, true, true,
			false, false, false, false,
			false, false, false, false
	};

	blocks[4].threeByThree = true;
	blocks[4].gameObject = new GameObject(rightBlockMesh, rightBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[4].localGrid = new bool[9];
	blocks[4].tempGrid = new bool[9];
	blocks[4].grid = new bool[9] {
		true, true, false,
			false, true, true,
			false, false, false
	};

	blocks[5].threeByThree = false;
	blocks[5].gameObject = new GameObject(squareBlockMesh, squareBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(2.0f, 2.0f, 0.0f));
	blocks[5].localGrid = new bool[16];
	blocks[5].tempGrid = new bool[16];
	blocks[5].grid = new bool[16] {
		false, false, false, false,
			false, true, true, false,
			false, true, true, false,
			false, false, false, false
	};

	blocks[6].threeByThree = true;
	blocks[6].gameObject = new GameObject(stairsBlockMesh, stairsBlockMaterial, &XMFLOAT3(0, 0, 0), &XMFLOAT3(0, 0, 0), &XMFLOAT3(1.5f, 1.5f, 0.0f));
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
	dataToSendToVSConstantBuffer.projection = projectionMatrix;
	dataToSendToVSConstantBuffer.lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.75f);

	// Update each mesh
	for (UINT i = 0; i < allObjects.size(); i++)
	{
		if (gameState != DEBUG)
			// [UPDATE] Update this object
			allObjects[i]->Update(dt);

		// [DRAW] Draw the object
		allObjects[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
	}

	// Update and draw the game if in game mode
	if (gameState == GAME)
	{
		blockManager->update(dt);
		blockManager->draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
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

	if (gameState == DEBUG)
	{
		// Strafing of camera
		if (GetAsyncKeyState(VK_LEFT))
			camera->MoveHorizontal(-CAMERA_MOVE_FACTOR * dt);
		else if (GetAsyncKeyState(VK_RIGHT))
			camera->MoveHorizontal(CAMERA_MOVE_FACTOR * dt);
		// Forward movement of camera
		if (GetAsyncKeyState(VK_UP))
			camera->MoveDepth(CAMERA_MOVE_FACTOR * dt);
		else if (GetAsyncKeyState(VK_DOWN))
			camera->MoveDepth(-CAMERA_MOVE_FACTOR * dt);
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
		case VK_SPACE:
			gameState = (gameState == MENU) ? GAME : MENU;

			if (gameState == MENU)
				allObjects = menuObjects;
			else if (gameState == GAME)
				allObjects = gameObjects;
			break;

			// Movement of game object
		case VK_NUMPAD8:
			allObjects[0]->Move(&XMFLOAT3(0.0f, 0.2f, 0.0f));
			break;
		case VK_NUMPAD5:
			allObjects[0]->Move(&XMFLOAT3(0.0f, -0.2f, 0.0f));
			break;
		case VK_NUMPAD6:
			allObjects[0]->Move(&XMFLOAT3(0.2f, 0.0f, 0.0f));
			break;
		case VK_NUMPAD4:
			allObjects[0]->Move(&XMFLOAT3(-0.2f, 0.0f, 0.0f));
			break;
			// Rotation of game object
		case VK_NUMPAD7:
			allObjects[0]->Rotate(&XMFLOAT3(0.0f, 0.0f, XM_PI / 2));
			break;
		case VK_NUMPAD9:
			allObjects[0]->Rotate(&XMFLOAT3(0.0f, 0.0f, -XM_PI / 2));
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