#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}

// Restart the particle system
void ParticleSystem::Reset()
{
	firstRun = true;
	age = 0.0f;
}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera& cam)
{
	// Grab the current view-projection
	XMMATRIX VP = XMMatrixMultiply(XMLoadFloat4x4(&cam.viewMatrix), XMLoadFloat4x4(&cam.projectionMatrix));

	// Update constant buffer with camera info

	// Set necessary settings for drawing

	// Draw mesh
		// This makes the vertex shader do its thing
		// Then the geometry shader will do its thing following that and spit out to PS
}

// TODO
	// Implement init
		// Create vertex buffers, etc
	// Figure what the ParticleEffect type is, and convert if necessary
	// Convert desired fx/technique to a .hlsl