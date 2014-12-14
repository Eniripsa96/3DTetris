#ifndef MESH_H
#define MESH_H

#include <vector>
#include <stdlib.h>
#include <time.h>

#include "Material.h"

using namespace std;
using namespace DirectX;

enum SHAPE
{
	NONE = 0,
	TRIANGLE = 1,
	QUAD = 2,
	PARTICLE = 20
};

// Vertex struct for triangles
struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

// Vertex struct for particles
struct Particle
{
	XMFLOAT3 initialPos;
	//XMFLOAT3 initialVel;
	XMFLOAT2 size;
	//float age;
	//unsigned int type;
};

// Struct to match vertex shader's constant buffer
// You update one of these locally, then push it to the corresponding
// constant buffer on the device when it needs to be updated
struct VertexShaderConstantBufferLayout
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT4X4 lightView;
	XMFLOAT4X4 lightProjection;
	XMFLOAT4 lightDirection;
	XMFLOAT4 color;
	XMFLOAT4 camPos;
};

// Struct to match particle geometry shader's constant buffer
// Update locally then push to corresponding buffer
struct GeometryShaderConstantBufferLayout
{
	XMFLOAT4X4 world;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT4 camPos;
	XMFLOAT4 age;
};

class Mesh
{
public:
	Mesh(ID3D11Device*, ID3D11DeviceContext*, SHAPE);
	Mesh(ID3D11Device*, ID3D11DeviceContext*, ID3D11Buffer*, ID3D11Buffer*, UINT);
	~Mesh();

	void CreateTrianglePoints();
	void CreateParticlePoints();
	void CreateQuadPoints();
	void CreateGeometryBuffers(Vertex[], Particle[]);
	void Draw();

	// Buffers to hold actual geometry
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	SHAPE shapeType;

	const XMFLOAT4 RED = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	const XMFLOAT4 GREEN = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	const XMFLOAT4 BLUE = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	const XMFLOAT3 NORMALS_2D = XMFLOAT3(0.0f, 0.0f, 1.0f);

	ID3D11DeviceContext* deviceContext;

private:
	ID3D11Device* device;
	UINT iBufferSize;
};

#endif