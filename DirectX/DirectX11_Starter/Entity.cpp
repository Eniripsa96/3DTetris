#include "Entity.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
Entity::Entity(ID3D11Device* device, ID3D11DeviceContext* context, Mesh* mesh, Material* mat)
{
	// Set mesh and material
	this->mesh = mesh;
	material = mat;
}

Entity::~Entity() { }

void Entity::Update(float dt)
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

void Entity::Draw()
{
	material->Draw();
	mesh->Draw();
}