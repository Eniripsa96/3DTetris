#include "GameObject.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* context, Mesh* mesh, Material* mat, XMFLOAT3* pos)
{
	// Set mesh and material
	this->mesh = mesh;
	material = mat;

	position = *pos;
}

GameObject::~GameObject() { }

void GameObject::Update(float dt)
{
	// Translate position
	if (mesh->shapeType == TRIANGLE)
		position.y += 0.1f * dt;
	else if (mesh->shapeType == QUAD)
		position.x += 0.1f * dt;
	
	// Apply translation to world matrix
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(translation /* * rotation * scale */));
}

void GameObject::Draw()
{
	material->Draw();
	mesh->Draw();
}