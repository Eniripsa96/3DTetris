#include "Mesh.h"
#include <d3dcompiler.h>

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, SHAPE type)
{
	// Grab some helpful data
	this->device = device;
	deviceContext = context;
	vertexShader = vShader;
	pixelShader = pShader;
	shapeType = type;

	// Initialize vertices based on shape type
	if (shapeType == TRIANGLE)
		CreateTrianglePoints();
	else if (shapeType == QUAD)
		CreateQuadPoints();

	//LoadShadersAndInputLayout();
}

Mesh::~Mesh()
{
	// Release all of the D3D stuff that's still hanging out
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	//ReleaseMacro(vsConstantBuffer);
	//ReleaseMacro(inputLayout);
}

void Mesh::CreateTrianglePoints()
{
	// Set up the vertices for a triangle
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), RED,	XMFLOAT2(0.5f, 0.0f)},
		{ XMFLOAT3(-1.0f, -0.5f, +0.0f), GREEN, XMFLOAT2(0.0f, 1.0f)},
		{ XMFLOAT3(+1.0f, -0.5f, +0.0f), BLUE,	XMFLOAT2(1.0f, 1.0f)},
	};

	CreateGeometryBuffers(vertices);
}

void Mesh::CreateQuadPoints()
{
	// Set up the vertices for a quad
	Vertex vertices[] =
	{
		// NOTE TEXTURE COORDS ARE 0,0 TOP LEFT AND 1,1 BOTTOM RIGHT
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), RED,	XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), GREEN, XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), BLUE,	XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(+1.0f, +0.0f, +0.0f), RED,	XMFLOAT2(1.0f, 1.0f) },
	};

	CreateGeometryBuffers(vertices);
}

// Creates the vertex and index buffers for a single triangle
void Mesh::CreateGeometryBuffers(Vertex vertices[])
{
	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 3 * (int)shapeType; // Number of vertices in the "model" you want to draw
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;
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

void Mesh::Update()
{

}

void Mesh::Draw()
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &(vertexBuffer), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Finally do the actual drawing
	deviceContext->DrawIndexed(
		3 * (int)this->shapeType,	// The number of indices we're using in this draw
		0,
		0);
}