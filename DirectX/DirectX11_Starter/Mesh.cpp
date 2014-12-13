#include "Mesh.h"
#include <d3dcompiler.h>

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, SHAPE type)
{
	// Grab some helpful data
	this->device = device;
	deviceContext = context;
	shapeType = type;

	// Initialize vertices based on shape type
	if (shapeType == TRIANGLE)
		CreateTrianglePoints();
	else if (shapeType == QUAD)
		CreateQuadPoints();
	else if (shapeType == PARTICLE)
		CreateParticlePoints();
}

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Buffer* pVertexBuffer, ID3D11Buffer* pIndexBuffer, UINT iBufferSize)
{
	this->device = device;
	deviceContext = context;
	vertexBuffer = pVertexBuffer;
	indexBuffer = pIndexBuffer;
	this->iBufferSize = iBufferSize;
}

Mesh::~Mesh()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
}

void Mesh::CreateTrianglePoints()
{
	// Set up the vertices for a triangle
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), NORMALS_2D, XMFLOAT2(0.5f, 0.0f) },
		{ XMFLOAT3(-1.0f, -0.5f, +0.0f), NORMALS_2D, XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+1.0f, -0.5f, +0.0f), NORMALS_2D, XMFLOAT2(1.0f, 1.0f) },
	};

	CreateGeometryBuffers(vertices, NULL);
}

void Mesh::CreateParticlePoints()
{
	// Set up points for particles
	Particle particles[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), XMFLOAT2(0.5f, 0.5f) },
		{ XMFLOAT3(-1.0f, -0.5f, +0.0f), XMFLOAT2(0.5f, 0.5f) },
		{ XMFLOAT3(+1.0f, -0.5f, +0.0f), XMFLOAT2(0.5f, 0.5f) },
	};

	CreateGeometryBuffers(NULL, particles);
}

// Create the points for a quad. Particle=true means this quad is for a particle system
void Mesh::CreateQuadPoints()
{
	// Set up the vertices for a quad
	Vertex vertices[] =
	{
		// NOTE TEXTURE COORDS ARE 0,0 TOP LEFT AND 1,1 BOTTOM RIGHT
		{ XMFLOAT3(0, 1, 0), NORMALS_2D, XMFLOAT2(0.0f, 0.0f) },	// Top left
		{ XMFLOAT3(0, 0, 0), NORMALS_2D, XMFLOAT2(0.0f, 1.0f) },	// Bottom left
		{ XMFLOAT3(1, 1, 0), NORMALS_2D, XMFLOAT2(1.0f, 0.0f) },	// Top right
		{ XMFLOAT3(1, 0, 0), NORMALS_2D, XMFLOAT2(1.0f, 1.0f) },	// Bottom right
	};

	CreateGeometryBuffers(vertices, NULL);
}

// Creates the vertex and index buffers for a single triangle. Dynamic=true means a dynamic vertex buffer
void Mesh::CreateGeometryBuffers(Vertex vertices[], Particle particles[])
{
	// Create a dynamic vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;

	if (vertices)
	{
		vbd.ByteWidth = sizeof(Vertex) * 3 * (int)shapeType; // Number of vertices in the "model" you want to draw
		initialVertexData.pSysMem = vertices;
	}
	else if (particles)
	{
		vbd.ByteWidth = sizeof(Particle) * 3 * (int)shapeType; // Number of vertices in the "model" you want to draw
		initialVertexData.pSysMem = particles;
	}

	HR(device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer));

	// Set up the indices
	UINT indices[] = { 0, 2, 1, 1, 2, 3 };

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 3 * (int)shapeType; // Number of indices in the "model" you want to draw
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer));
}

void Mesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = (shapeType != PARTICLE) ? sizeof(Vertex) : sizeof(Particle);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	if (shapeType != NONE)
	{
		// Finally do the actual drawing
		if (shapeType != PARTICLE)
			deviceContext->DrawIndexed(
				3 * (int)this->shapeType,	// The number of indices we're using in this draw
				0,
				0);
		else
			deviceContext->DrawIndexed(
				3,	// The number of indices we're using in this draw
				0,
				0);
	}
	else
	{
		deviceContext->DrawIndexed(
			iBufferSize,	// The number of indices we're using in this draw
			0,
			0);
	}
}