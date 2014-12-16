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
	srand(time(NULL));

	vector<Particle> particles;
	
	for (int i = 0; i < (int)PARTICLE; i++)
	{
		particles.push_back(Particle{ XMFLOAT3((rand() % 100) / 10.0f - 5.0f, ((rand() % 50) / 10.0f - 2.5f) - 4.0f, -1.0f), XMFLOAT2(PARTICLE_SIZE, PARTICLE_SIZE) });
	}
	CreateGeometryBuffers(NULL, &particles[0]);
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

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;

	if (vertices)
	{
		vbd.ByteWidth = sizeof(Vertex) * 3 * (int)shapeType; // Number of vertices in the "model" you want to draw
		initialVertexData.pSysMem = vertices;

		// Set up the indices
		UINT indices[] = { 0, 2, 1, 1, 2, 3 };
		initialIndexData.pSysMem = indices;
		ibd.ByteWidth = sizeof(UINT) * 3 * (int)shapeType; // Number of indices in the "model" you want to draw
	}
	else if (particles)
	{
		vbd.ByteWidth = sizeof(Particle) * (int)PARTICLE; // Number of vertices in the "model" you want to draw
		initialVertexData.pSysMem = particles;

		UINT particleIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49 };
		/*vector<UINT> particleIndices;
		for (int i = 0; i < (int)PARTICLE; i++)
			particleIndices.push_back(i);*/
		initialIndexData.pSysMem = particleIndices;
		ibd.ByteWidth = sizeof(UINT) * (int)PARTICLE; // Number of indices in the "model" you want to draw
	}

	HR(device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer));

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
				(int)PARTICLE,	// The number of indices we're using in this draw
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