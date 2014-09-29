#include "Entity.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
Entity::Entity(ID3D11Device* device, ID3D11DeviceContext* context, Material* mat, ID3D11VertexShader* vShader, ID3D11PixelShader* pShader, SHAPE type)
{
	// Set mesh and material
	mesh = new Mesh(device, context, type);
	material = mat;

	// Set shaders
	mesh->vertexShader = vShader;
	mesh->pixelShader = pShader;

	// NOTE: This will be removed when different materials have different shaders
	material->vertexShader = vShader;
	material->pixelShader = pShader;
}

Entity::~Entity()
{
	delete mesh;
	delete material;
}

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