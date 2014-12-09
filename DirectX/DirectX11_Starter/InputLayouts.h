#ifndef INPUTLAYOUTS_H
#define INPUTLAYOUTS_H

#include <d3d11.h>

class InputLayouts
{
public:
	InputLayouts();
	~InputLayouts();

	static void InitializeVertexLayout(ID3D11Device*, ID3DBlob*);
	static void InitializeParticleLayout(ID3D11Device*, ID3DBlob*);

	static ID3D11InputLayout* Vertex;
	static ID3D11InputLayout* Particle;
};

#endif