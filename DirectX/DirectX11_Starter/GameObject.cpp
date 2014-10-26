#include "GameObject.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
GameObject::GameObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel)
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
	//position.x += velocity.x * dt;
	//position.y += velocity.y * dt;
	
	// Apply translation to world matrix
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(rotate * translation /** scale */));
}

void GameObject::Move(XMFLOAT3* move)
{
	position.x += move->x;
	position.y += move->y;
	position.z += move->z;
}

void GameObject::Rotate(XMFLOAT3* rotate)
{
	rotation.x += rotate->x;
	rotation.y += rotate->y;
	rotation.z += rotate->z;
}

void GameObject::Draw()
{
	material->Draw();
	mesh->Draw();
}