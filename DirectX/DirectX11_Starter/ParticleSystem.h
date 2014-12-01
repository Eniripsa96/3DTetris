#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <utility>

#include "Camera.h"

using namespace DirectX;

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	// Time elapsed since the systm was reset
	float GetAge() const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Init(ID3D11Device* device, float fx /* shader??? */, ID3D11ShaderResourceView* texArraySRV, ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam);

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

	XMFLOAT3 eyePosW;
	XMFLOAT3 emitPosW;
	XMFLOAT3 emitDirW;

	float fx;	// shader???

	ID3D11Buffer* initVB;
	ID3D11Buffer* drawVB;
	ID3D11Buffer* streamOutVB;

	ID3D11ShaderResourceView* texArraySRV;
	ID3D11ShaderResourceView* randomTexSRV;
};

#endif