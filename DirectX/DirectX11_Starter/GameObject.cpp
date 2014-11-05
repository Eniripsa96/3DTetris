#include "GameObject.h"

// Constructor gives us device, device context, a material, shaders, and a shape type
GameObject::GameObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel)
{
	// Set mesh and material
	this->mesh = mesh;
	material = mat;
	velocity = *vel;
	position = *pos;
	pivot = XMFLOAT3(0, 0, 0);
}

// Constructor gives us device, device context, a material, shaders, and a shape type
GameObject::GameObject(Mesh* mesh, Material* mat, XMFLOAT3* pos, XMFLOAT3* vel, XMFLOAT3* pPivot)
{
	// Set mesh and material
	this->mesh = mesh;
	material = mat;
	velocity = *vel;
	position = *pos;
	pivot = *pPivot;
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
	XMMATRIX pivotMatrix = XMMatrixTranslation(pivot.x, pivot.y, pivot.z);
	XMMATRIX inversePivotMatrix = XMMatrixTranslation(-pivot.x, -pivot.y, -pivot.z);

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(inversePivotMatrix * rotate * pivotMatrix * translation /** scale */));
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

void GameObject::ClearRotation()
{
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext, ID3D11Buffer* cBuffer, VertexShaderConstantBufferLayout* cBufferData)
{
	cBufferData->world = worldMatrix;

	// [UPDATE] Update the constant buffer itself
	deviceContext->UpdateSubresource(
		cBuffer,
		0,
		NULL,
		cBufferData,
		0,
		0
		);

	// [DRAW] Set the constant buffer in the device
	deviceContext->VSSetConstantBuffers(
		0,
		1,
		&(cBuffer)
		);

	material->Draw();
	mesh->Draw();
}