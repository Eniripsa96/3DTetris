#include "GameObject.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* context, Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel)
{
	// Set mesh and material
	this->mesh = mesh;
	material = mat;
	velocity = *vel;
	position = *pos;
}

GameObject::~GameObject() { }

void GameObject::Update(float dt)
{
	// Update position via velocity
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	
	// Apply translation to world matrix
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(translation /* * rotation * scale */));
}

void GameObject::Draw()
{
	material->Draw();
	mesh->Draw();
}