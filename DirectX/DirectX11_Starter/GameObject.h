#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Mesh.h"

class GameObject
{
public:
	GameObject(ID3D11Device*, ID3D11DeviceContext*, Mesh*, Material*, XMFLOAT3*);
	~GameObject();

	void Update(float);
	void Draw(void);

	Mesh* mesh;
	Material* material;

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	XMFLOAT4X4 worldMatrix;
};

#endif