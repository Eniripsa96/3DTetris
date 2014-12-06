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

	// Change Look
	void Move(XMFLOAT3*);
	void MoveTo(XMFLOAT3*);
	void MoveVertical(float);
	void MoveHorizontal(float);
	void MoveDepth(float);
	void RotateY(float);
	void Pitch(float);
	void Reset();

	// Accessors
	XMVECTOR GetForwardXM() const;
	XMFLOAT3 GetForward() const;
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;

	// The matrices to go from model space
	// to screen space
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

private:
	XMFLOAT3 position;
	XMFLOAT3 target;

	// Basis vectors
	XMFLOAT3 forward;
	XMFLOAT3 right;
	XMFLOAT3 up;
};

#endif