#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void Update(float);
	void Move(XMFLOAT4*);

	// The matrices to go from model space
	// to screen space
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

private:
	XMVECTOR position;
	XMVECTOR target;
	XMVECTOR up;

	XMFLOAT4 myPosition;
};

#endif