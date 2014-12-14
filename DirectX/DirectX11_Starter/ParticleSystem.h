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

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Init(ID3D11Device* device, float fx /* shader??? */, ID3D11ShaderResourceView* texArraySRV, ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles);

	void Reset();
	void Draw(ID3D11DeviceContext* dc, const Camera& cam, ID3D11Buffer* cBuffer, GeometryShaderConstantBufferLayout* cBufferData);
	void Update(GeometryShaderConstantBufferLayout* cBufferData, float dt);

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

private:
	UINT maxParticles;
	bool firstRun;

	float gameTime;
	float timeStep;
	float age;

	XMFLOAT3 particles[3];

	Mesh* mesh;
	Material* material;

	XMFLOAT4X4 world;

	const float INITIAL_AGE = 10.0f;
};

#endif