#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Mesh.h"

class GameObject
{
public:
	GameObject(ID3D11Device*, ID3D11DeviceContext*, Mesh*, Material*);
	~GameObject();

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