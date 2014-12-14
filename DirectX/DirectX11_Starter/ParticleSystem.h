#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>

#include "Camera.h"
#include "InputLayouts.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class ParticleSystem
{
public:
	ParticleSystem(Mesh*, Material*);
	~ParticleSystem();

	// Time elapsed since the systm was reset
	float GetAge() const;
	Material* GetMaterial() const;

	void Reset();
	void Draw(ID3D11DeviceContext* dc, const Camera& cam, ID3D11Buffer* cBuffer, GeometryShaderConstantBufferLayout* cBufferData);
	void Update(GeometryShaderConstantBufferLayout* cBufferData, float dt);

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

private:
	XMFLOAT3 particles[3];

	Mesh* mesh;
	Material* material;

	XMFLOAT4X4 world;
	float velocity;
	float age;

	const float INITIAL_VEL = 0.5f;
	const float GRAVITY = -2.0f;
	const float INITIAL_AGE = 10.0f;
};

#endif