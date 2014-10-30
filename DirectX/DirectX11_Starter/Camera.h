#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void Update(float);
	void Move(XMFLOAT3*);
	void MoveVertical(float);
	void MoveHorizontal(float);
	void MoveDepth(float);
	void Rotate(XMFLOAT3*);

	// The matrices to go from model space
	// to screen space
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

private:
	XMVECTOR position;
	XMVECTOR target;
	XMVECTOR up;

	XMFLOAT3 myPosition;
	XMFLOAT3 myTarget;

	XMVECTOR forward();
	XMFLOAT3 right();
	//XMFLOAT3 up();
};

#endif