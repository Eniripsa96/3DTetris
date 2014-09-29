#ifndef ENTITY_H
#define ENTITY_H

#include "Mesh.h"

class Entity
{
public:
	Entity(ID3D11Device*, ID3D11DeviceContext*, Material*, ID3D11VertexShader*, ID3D11PixelShader*, SHAPE);
	~Entity();

	void Update(float);
	void Draw(void);

	Mesh* mesh;
	Material* material;

	XMFLOAT4 position;
	XMFLOAT4 rotation;
	XMFLOAT4 scale;

	XMFLOAT4X4 worldMatrix;
};

#endif