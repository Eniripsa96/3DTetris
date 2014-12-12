#include "InputLayouts.h"

//bool CommandManager::started = true;
ID3D11InputLayout* InputLayouts::Vertex = NULL;
ID3D11InputLayout* InputLayouts::Particle = NULL;
ID3D11InputLayout* InputLayouts::Shadow = NULL;

InputLayouts::InputLayouts()
{
}


InputLayouts::~InputLayouts()
{
}

ID3D11InputLayout* InputLayouts::InitializeVertexLayout(ID3D11Device* device, ID3DBlob* vsBlob)
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	// Before cleaning up the data, create the input layout
	device->CreateInputLayout(
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&Vertex);

	return Vertex;
}

ID3D11InputLayout* InputLayouts::InitializeShadowLayout(ID3D11Device* device, ID3DBlob* vsBlob)
{
	// Shadow vertex description
	D3D11_INPUT_ELEMENT_DESC shadowDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Before cleaning up the data, create the input layout
	device->CreateInputLayout(
		shadowDesc,
		ARRAYSIZE(shadowDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&Shadow);

	return Shadow;
}

ID3D11InputLayout* InputLayouts::InitializeParticleLayout(ID3D11Device* device, ID3DBlob* vsBlob)
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC particleDesc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "VELOCITY",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "AGE",		0, DXGI_FORMAT_R32_FLOAT,			0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TYPE",		0, DXGI_FORMAT_R32_UINT,			0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Before cleaning up the data, create the input layout
	device->CreateInputLayout(
		particleDesc,
		ARRAYSIZE(particleDesc),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&Particle);

	return Particle;
}