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

	// What is the FX business? Look up this class in the book
	// Set constants
	/*fx->SetViewProj(VP);
	fx->SetGameTime(gameTime);
	fx->SetTimeStep(timeStep);
	fx->SetEyePosW(eyePosW);
	fx->SetEmitPosW(emitPosW);
	fx->SetEmitDirW(emitDirW);
	fx->SetTexArray(texArraySRV);
	fx->SetRandomTex(randomTexSRV);*/

	// Set IA stage
	//dc->IASetInputLayout(InputLayouts::Particle);	// Need to create this layout
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = 0;
	//UINT stride = sizeof(Vertex::Particle);	// need to create this particle struct
	UINT offset = 0;

	// On the first pass, une the initialization VB. Otherwise, us the VB that contains the current particle list
	if (firstRun)
		dc->IASetVertexBuffers(0, 1, &initVB, &stride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);

	// Draw the current particle list using stream-out only to update them. The updated vertices are streamed-out to the target VB.
	dc->SOSetTargets(1, &streamOutVB, &offset);
	//fx->StreamOutTech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//fx->StreamOutTech->GetPassByIndex(p)->Apply(0, dc);

		if (firstRun)
		{
			dc->Draw(1, 0);
			firstRun = false;
		}
		else
		{
			dc->DrawAuto();
		}
	}

	// Done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	dc->SOSetTargets(1, bufferArray, &offset);

	// Ping-pong the vertex buffers
	std::swap(drawVB, streamOutVB);

	// Draw the updated particle system we just streamed-out
	dc->IASetVertexBuffers(0, 1, &drawVB, &stride, &offset);

	//fx->DrawTech->GetDesc(&techDesc);
	//for (UINT p = 0; p < techDesc.Passes; ++p)
	{
	//	fx->DrawTech->GetPassByIndex(p)->Apply(0, dc);

		dc->DrawAuto();
	}
	
}

// TODO
	// Implement init
		// Create vertex buffers, etc
	// Figure what the ParticleEffect type is, and convert if necessary
	// Convert desired fx/technique to a .hlsl