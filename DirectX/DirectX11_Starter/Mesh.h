#ifndef MESH_H
#define MESH_H

#include "Material.h"

using namespace DirectX;

enum SHAPE
{
	NONE = 0,
	TRIANGLE = 1,
	QUAD = 2
};

// Vertex struct for triangles
struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

// Struct to match vertex shader's constant buffer
// You update one of these locally, then push it to the corresponding
// constant buffer on the device when it needs to be updated
struct VertexShaderConstantBufferLayout
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

class Mesh
{
public:
	Mesh(ID3D11Device*, ID3D11DeviceContext*, SHAPE);
	Mesh(ID3D11Device*, ID3D11DeviceContext*, ID3D11Buffer*, ID3D11Buffer*, UINT);
	~Mesh();

	void CreateTrianglePoints();
	void CreateQuadPoints();
	void CreateGeometryBuffers(Vertex[]);
	void Update();
	void Draw();

	// Buffers to hold actual geometry
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	SHAPE shapeType;

	const XMFLOAT4 RED = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 GREEN = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 BLUE = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	const XMFLOAT3 NORMALS_2D = XMFLOAT3(0.0f, 0.0f, 1.0f);

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	UINT iBufferSize;
};

#endif