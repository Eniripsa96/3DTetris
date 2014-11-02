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

	// Test OBJ loader
	
	ObjLoader* loader = new ObjLoader();
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT i = loader->Load("cube.txt", device, &vertexBuffer, &indexBuffer);
	delete loader;
	cubeMesh = new Mesh(device, deviceContext, vertexBuffer, indexBuffer, i);

	// Create meshes
	triangleMesh = new Mesh(device, deviceContext, TRIANGLE);
	quadMesh = new Mesh(device, deviceContext, QUAD);

	// Create materials
	shapeMaterial = new Material(device, deviceContext, vertexShader, pixelShader, L"image.png");

	// Create the game objects we want
	gameObjects.emplace_back(new GameObject(triangleMesh,	shapeMaterial, &XMFLOAT3(0.0f, -1.0f, 0.0f), &XMFLOAT3(0.1f, 0.0f, 0.0f)));
	gameObjects.emplace_back(new GameObject(quadMesh,		shapeMaterial, &XMFLOAT3(-1.0f, 0.0f, 0.0f), &XMFLOAT3(0.1f, 0.0f, 0.0f)));

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

	// Update each mesh
	for (UINT i = 0; i < allObjects.size(); i++)
	{
		// [UPDATE] Update this object
		allObjects[i]->Update(dt);

		// [UPDATE] Update constant buffer data using this object
		dataToSendToVSConstantBuffer.world = allObjects[i]->worldMatrix;
		dataToSendToVSConstantBuffer.view = camera->viewMatrix;
		dataToSendToVSConstantBuffer.projection = projectionMatrix;
		dataToSendToVSConstantBuffer.lightDirection = XMFLOAT4(2.0f, -3.0f, 1.0f, 0.75f);

		// [UPDATE] Update the constant buffer itself
		deviceContext->UpdateSubresource(
			vsConstantBuffer,
			0,
			NULL,
			&dataToSendToVSConstantBuffer,
			0,
			0);

		// [DRAW] Set the constant buffer in the device
		deviceContext->VSSetConstantBuffers(
			0,	// Corresponds to the constant buffer's register in the vertex shader
			1,
			&(vsConstantBuffer));

		// [DRAW] Draw the object
		allObjects[i]->Draw();
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

	// Horizontal rotation of camera
	if (GetAsyncKeyState('J'))
		camera->RotateY(CAMERA_TURN_FACTOR * dt);
	else if (GetAsyncKeyState('L'))
		camera->RotateY(-CAMERA_TURN_FACTOR * dt);
	// Vertical rotation of camera
	if (GetAsyncKeyState('I'))
		camera->Pitch(CAMERA_TURN_FACTOR * dt);
	else if (GetAsyncKeyState('K'))
		camera->Pitch(-CAMERA_TURN_FACTOR * dt);
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