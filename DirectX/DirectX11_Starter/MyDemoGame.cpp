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
#include "MyDemoGame.h"

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
	MyDemoGame game(hInstance);
	
	if( !game.Init() )
		return 0;
	
	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

MyDemoGame::MyDemoGame(HINSTANCE hInstance) : DirectXGame(hInstance)
{
	// Set up our custom caption and window size
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;
}

MyDemoGame::~MyDemoGame()
{
	// Clean up here
	for (UINT i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}

	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(inputLayout);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool MyDemoGame::Init()
{
	if( !DirectXGame::Init() )
		return false;

	// Load shaders that we want
	LoadShadersAndInputLayout();

	// Create the shapes we want
	entities.emplace_back(new Entity(device, deviceContext, new Material(device, deviceContext), vertexShader, pixelShader, TRIANGLE));
	entities.emplace_back(new Entity(device, deviceContext, new Material(device, deviceContext), vertexShader, pixelShader, QUAD));

	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	XMVECTOR position	= XMVectorSet(0, 0, -5, 0);
	XMVECTOR target		= XMVectorSet(0, 0, 0, 0);
	XMVECTOR up			= XMVectorSet(0, 1, 0, 0);
	XMMATRIX V			= XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	// Set up the world matrix for each mesh
	XMMATRIX W = XMMatrixIdentity();
	for (UINT i = 0; i < entities.size(); i++)
		XMStoreFloat4x4(&(entities[i]->worldMatrix), XMMatrixTranspose(W));

	return true;
}

// TODO: EXTRACT CODE TO MAIN GAME AND PASS SHADER IN, THEN GIVE MESH REFERENCE TO SHADER
// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void MyDemoGame::LoadShadersAndInputLayout()
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

#pragma region Window Resizing

// Handles resizing the window and updating our projection matrix to match
void MyDemoGame::OnResize()
{
	// Handle base-level DX resize stuff
	DirectXGame::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
}
#pragma endregion

#pragma region Game Loop

// Updates the buffers (and pushes them to the buffer on the device)
// and draws for each entity
void MyDemoGame::UpdateScene(float dt)
{
	// Clear the buffer
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Update each mesh
	for (UINT i = 0; i < entities.size(); i++)
	{
		// Update this entity
		entities[i]->Update(dt);

		// Update constant buffer data
		dataToSendToVSConstantBuffer.world = entities[i]->worldMatrix;
		dataToSendToVSConstantBuffer.view = viewMatrix;
		dataToSendToVSConstantBuffer.projection = projectionMatrix;

		// Update the constant buffer itself
		deviceContext->UpdateSubresource(
			vsConstantBuffer,
			0,
			NULL,
			&dataToSendToVSConstantBuffer,
			0,
			0);

		// Set the constant buffer in the device
		deviceContext->VSSetConstantBuffers(
			0,	// Corresponds to the constant buffer's register in the vertex shader
			1,
			&(vsConstantBuffer));

		// Set up the input assembler
		deviceContext->IASetInputLayout(inputLayout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw the entity
		entities[i]->Draw();
	}

	// Present the buffer
	HR(swapChain->Present(0, 0));
}

// NOTE: DEPRECATED
// Clear the screen, redraw everything, present
void MyDemoGame::DrawScene()
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

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input

void MyDemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;

	SetCapture(hMainWnd);
}

void MyDemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MyDemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;
}
#pragma endregion