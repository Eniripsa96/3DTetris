#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(Mesh* mesh, Material* mat)
{
	this->mesh = mesh;
	this->material = mat;

	age = 0.0f;

	XMFLOAT3 particles[3] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f) },
		{ XMFLOAT3(-1.0f, -0.5f, +0.0f) },
		{ XMFLOAT3(+1.0f, -0.5f, +0.0f) },
	};

	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
}


ParticleSystem::~ParticleSystem()
{
	
}

// Restart the particle system
void ParticleSystem::Reset()
{
	age = INITIAL_AGE;
	velocity = INITIAL_VEL;
	XMStoreFloat4x4(&world, (XMMatrixTranslation(0.0f, 0.0f, 0.0f)));
}

Material* ParticleSystem::GetMaterial() const
{
	return this->material;
}

void ParticleSystem::Update(GeometryShaderConstantBufferLayout* cBufferData, float dt)
{
	// Grab the current view-projection
	//XMMATRIX VP = XMMatrixMultiply(XMLoadFloat4x4(&cam.viewMatrix), XMLoadFloat4x4(&cam.projectionMatrix));

	age -= 1.0f * dt;
	cBufferData->age = XMFLOAT4(age, 0, 0, 0);

	velocity += GRAVITY * dt;

	XMMATRIX tempWorld = XMMatrixTranslation(0.0f, velocity * dt, 0.0f );
	XMStoreFloat4x4(&world, XMLoadFloat4x4(&world) * tempWorld);
	cBufferData->world = world;
}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera& cam, ID3D11Buffer* cBuffer, GeometryShaderConstantBufferLayout* cBufferData)
{
	// Update constant buffer with camera info
	cBufferData->camPos = cam.GetPos();

	// [UPDATE] Update the constant buffer itself
	dc->UpdateSubresource(
		cBuffer,
		0,
		NULL,
		cBufferData,
		0,
		0
		);

	// [DRAW] Set the constant buffer in the device
	dc->GSSetConstantBuffers(
		0,
		1,
		&(cBuffer)
		);

	// Draw mesh and material
	material->Draw();
	mesh->Draw();
}

// TODO
	// Implement init
		// Create vertex buffers, etc
	// Figure what the ParticleEffect type is, and convert if necessary
	// Convert desired fx/technique to a .hlsl