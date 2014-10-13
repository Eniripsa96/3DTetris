#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Mesh.h"

class GameObject
{
public:
	GameObject(ID3D11Device*, ID3D11DeviceContext*, Mesh*, Material*, XMFLOAT3*, XMFLOAT3*);
	~GameObject();

	void Update(float);
	void Draw(void);
	void Move(XMFLOAT3*);
	void Rotate(XMFLOAT3*);	

	Mesh* mesh;
	Material* material;

	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	XMFLOAT3 velocity;

	XMFLOAT4X4 worldMatrix;
};

#endif