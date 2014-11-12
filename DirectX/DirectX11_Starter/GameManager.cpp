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

GameManager::~GameManager()
{
	// Clean up here
	for (UINT i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}
	for (UINT i = 0; i < menuObjects.size(); i++)
	{
		delete menuObjects[i];
	}

	delete triangleMesh;
	delete quadMesh;
	delete cubeMesh;

	delete shapeMaterial;

	delete camera;

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

	// Create materials
	shapeMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"image.png");
	uiTestMaterial = new Material(device, deviceContext, uiVertexShader, uiPixelShader, L"button.png");

	// Prepare to load meshes
	ObjLoader* loader = new ObjLoader();
	
	// Load basic cube
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT i = loader->Load("cube.txt", device, &vertexBuffer, &indexBuffer);
	cubeMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, i);

	// Set up block types
	blocks = new Block[7];

	int size = loader->Load("jBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[0].threeByThree = true;
	blocks[0].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texJBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[0].localGrid = new bool[9];
	blocks[0].tempGrid = new bool[9];
	blocks[0].grid = new bool[] {
		true,  true,  true,
		true,  false, false,
		false, false, false
	};

	size = loader->Load("lBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[1].threeByThree = true;
	blocks[1].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texLBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[1].localGrid = new bool[9];
	blocks[1].tempGrid = new bool[9];
	blocks[1].grid = new bool[] {
		true,  true,  true,
		false, false, true,
		false, false, false
	};

	size = loader->Load("leftBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[2].threeByThree = true;
	blocks[2].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texLeftBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[2].localGrid = new bool[9];
	blocks[2].tempGrid = new bool[9];
	blocks[2].grid = new bool[] {
		false, true,  true,
		true,  true,  false,
		false, false, false
	};

	size = loader->Load("longBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[3].threeByThree = false;
	blocks[3].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texLongBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(2.0f, 2.0f, 0.0f));
	blocks[3].localGrid = new bool[16];
	blocks[3].tempGrid = new bool[16];
	blocks[3].grid = new bool[] {
		false, false, false, false,
		true,  true,  true,  true,
		false, false, false, false,
		false, false, false, false
	};

	size = loader->Load("rightBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[4].threeByThree = true;
	blocks[4].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texRightBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[4].localGrid = new bool[9];
	blocks[4].tempGrid = new bool[9];
	blocks[4].grid = new bool[] {
		true,  true,  false,
		false, true,  true,
		false, false, false
	};

	size = loader->Load("squareBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[5].threeByThree = false;
	blocks[5].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texSquareBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(2.0f, 2.0f, 0.0f));
	blocks[5].localGrid = new bool[16];
	blocks[5].tempGrid = new bool[16];
	blocks[5].grid = new bool[] {
		false, false, false, false,
		false, true,  true,  false,
		false, true,  true,  false,
		false, false, false, false
	};

    size = loader->Load("stairsBlock.txt", device, &vertexBuffer, &indexBuffer);
	blocks[6].threeByThree = true;
	blocks[6].gameObject = new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texStairsBlock.png"), new XMFLOAT3(0, 0, 0), new XMFLOAT3(0, 0, 0), new XMFLOAT3(1.5f, 1.5f, 0.0f));
	blocks[6].localGrid = new bool[9];
	blocks[6].tempGrid = new bool[9];
	blocks[6].grid = new bool[] {
		true,  true,  true,
		false, true,  false,
		false, false, false
	};

	spriteBatch = new SpriteBatch(deviceContext);
	spriteFont32 = new SpriteFont(device, L"jing32.spritefont");
	spriteFont72 = new SpriteFont(device, L"jing72.spritefont");

	// Load the frame
	size = loader->Load("frame.txt", device, &vertexBuffer, &indexBuffer);
	gameObjects.emplace_back(new GameObject(new Mesh(device, deviceContext, vertexBuffer, indexBuffer, size), new Material(device, deviceContext, vertexShader, pixelShader, L"texFrame.png"), new XMFLOAT3(-3.0f, -5.0f, 0.0f), new XMFLOAT3(0.0f, 0.0f, 0.0f)));

	// Finished using the ObjLoader
	delete loader;

	// Set up block manager
	for (int j = 0; j < GRID_HEIGHT; j++) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			cubes.push_back(*new GameObject(cubeMesh, shapeMaterial, new XMFLOAT3(i - 4.5, j - 5, 0), new XMFLOAT3(0, 0, 0)));
		}
	}
	blockManager = new BlockManager(blocks, 7, cubes, XMFLOAT3(-5, -5, 0), XMFLOAT3(-8.25, 12.5, 0), 1);
	blockManager->spawnFallingBlock();

	// Create 2D meshes
	//triangleMesh = new Mesh(device, deviceContext, TRIANGLE);
	quadMesh = new Mesh(device, deviceContext, QUAD);
	Button* obj = new Button(quadMesh, uiTestMaterial, new XMFLOAT3(0, 0, 0), 500, 150, spriteBatch, spriteFont32, L"Play");
	obj->Scale(&XMFLOAT3(1.0f, 0.3f, 1.0f));
	menuObjects.emplace_back(obj);

	// Create the game objects we want
	//gameObjects.emplace_back(new GameObject(triangleMesh,	shapeMaterial, &XMFLOAT3(0.0f, -1.0f, 0.0f), &XMFLOAT3(0.1f, 0.0f, 0.0f)));
	//gameObjects.emplace_back(new GameObject(quadMesh,		shapeMaterial, &XMFLOAT3(-1.0f, 0.0f, 0.0f), &XMFLOAT3(0.1f, 0.0f, 0.0f)));

	// Create the menu objects we want
	//menuObjects.emplace_back(new GameObject(cubeMesh, shapeMaterial, &XMFLOAT3(0.0f, -0.0f, 0.0f), &XMFLOAT3(0.0f, 0.0f, 0.0f)));

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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
	
	// Load UI Vertex Shader -----------------------------------
	D3DReadFileToBlob(L"uiVertexShader.cso", &vsBlob);

	// Create the shader on the device
	HR(device->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		NULL,
		&uiVertexShader));

	// Clean up
	ReleaseMacro(vsBlob);

	// Load UI Pixel Shader -------------------------------------
	D3DReadFileToBlob(L"uiPixelShader.cso", &psBlob);

	// Create the shader on the device
	HR(device->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		NULL,
		&uiPixelShader));

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
	//dataToSendToVSConstantBuffer.resolution = XMFLOAT2(windowWidth, windowHeight);

	// Projection matrix
	if (gameState == GAME) {
		dataToSendToVSConstantBuffer.projection = projectionMatrix;
	}
	else {
		dataToSendToVSConstantBuffer.projection = uiProjectionMatrix;
	}

	// Update each mesh
	spriteBatch->Begin();
	for (UINT i = 0; i < allObjects.size(); i++)
	{
		// [UPDATE] Update this object
		allObjects[i]->Update(dt);

		// [DRAW] Draw the object
		allObjects[i]->Draw(deviceContext, vsConstantBuffer, &dataToSendToVSConstantBuffer);
	}
	spriteBatch->End();

	// Update and draw the game if in game mode
	if (gameState == GAME) {
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

	//else
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
	if ((btnState & MK_LBUTTON) != 0)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - prevMousePos.x));
		camera->RotateY(-dx);
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

	XMMATRIX T = XMMatrixTranslation(-1, -1, 0);
	XMMATRIX UP = XMMatrixScaling(2.0f / windowWidth, /*2.0f*/ 6.0f / windowHeight, 1);
	XMStoreFloat4x4(&uiProjectionMatrix, UP); // * T);
}
#pragma endregion