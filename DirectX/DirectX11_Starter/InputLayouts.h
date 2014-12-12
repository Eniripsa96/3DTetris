#ifndef INPUTLAYOUTS_H
#define INPUTLAYOUTS_H

#include <d3d11.h>

enum LAYOUT
{
	VERTEX_LAYOUT,
	PARTICLE_LAYOUT,
	SHADOW_LAYOUT
};

class InputLayouts
{
public:
	InputLayouts();
	~InputLayouts();

	static ID3D11InputLayout* InitializeVertexLayout(ID3D11Device*, ID3DBlob*);
	static ID3D11InputLayout* InitializeShadowLayout(ID3D11Device*, ID3DBlob*);
	static ID3D11InputLayout* InitializeParticleLayout(ID3D11Device*, ID3DBlob*);

	static ID3D11InputLayout* Vertex;
	static ID3D11InputLayout* Shadow;
	static ID3D11InputLayout* Particle;
};

#endif