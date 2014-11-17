#include "Camera.h"
#include <iostream>

using namespace std;

Camera::Camera()
{
	float xPos = -10.0f;
	float xTar = 0.0f;
	float yPos = 0.0f;
	float yTar = 12.0f;
	float zPos = -40.0f;
	float zTar = 10.0f;
	
	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	position = XMFLOAT3(0.0f, 0.0f, zPos);
	target = XMFLOAT3(0.0f, 0.0f, zTar);
	XMMATRIX V = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), GetUpXM());
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	forward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	right = XMFLOAT3(1.0f, 0.0f, 0.0f);

	projectionMatrix = XMFLOAT4X4();
}

Camera::~Camera()
{
}

void Camera::Move(XMFLOAT3* move)
{
	// Move position and target to properly move camera by move vector

	position.x += move->x / 10.0f;
	position.y += move->y / 10.0f;
	position.z += move->z / 10.0f;

	target.x += move->x / 10.0f;
	target.y += move->y / 10.0f;
	target.z += move->z / 10.0f;
}

void Camera::MoveVertical(float move)
{
	// Translate camera along up vector
	XMVECTOR s = XMVectorReplicate(move);
	XMVECTOR u = XMLoadFloat3(&up);
	XMVECTOR p = XMLoadFloat3(&position);

	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, u, p));
}

void Camera::MoveHorizontal(float move)
{
	// Translate camera along right vector
	XMVECTOR s = XMVectorReplicate(move);
	XMVECTOR r = XMLoadFloat3(&right);
	XMVECTOR p = XMLoadFloat3(&position);
	XMVECTOR t = XMLoadFloat3(&target);

	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
	XMStoreFloat3(&target, XMVectorMultiplyAdd(s, r, t));
}

void Camera::MoveDepth(float move)
{
	// Translate camera along forward vector
	XMVECTOR s = XMVectorReplicate(move);
	XMVECTOR f = XMLoadFloat3(&forward);
	XMVECTOR p = XMLoadFloat3(&position);
	XMVECTOR t = XMLoadFloat3(&target);

	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, f, p));
	XMStoreFloat3(&target, XMVectorMultiplyAdd(s, f, t));
}

void Camera::Pitch(float angle)
{
	// Get rotation about right vector
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), -1.0f * angle);

	// Rotate basis vectors about right vector
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));

	// Rotate target about right vector for LookAt function
	XMStoreFloat3(&target, XMVector3TransformNormal(XMLoadFloat3(&target), R));
}

void Camera::RotateY(float angle)
{
	// Get rotation about Y axis
	XMMATRIX R = XMMatrixRotationY(-1.0f * angle);

	// Rotate basis vectors about Y axis
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));

	// Rotate target about Y axis for LookAt function
	XMStoreFloat3(&target, XMVector3TransformNormal(XMLoadFloat3(&target), R));
}

// Rebuild view matrix
void Camera::Update(float dt)
{
	XMVECTOR R = XMLoadFloat3(&right);
	XMVECTOR U = XMLoadFloat3(&up);
	XMVECTOR F = XMLoadFloat3(&forward);
	XMVECTOR P = XMLoadFloat3(&position);

	// Orthonormalize the right, up, and look vectors

	// Normalize forward to unit length
	F = XMVector3Normalize(F);

	// Compute new up vector and normalize
	U = XMVector3Normalize(XMVector3Cross(F, R));

	// Compute new right vector. U and F are already ortho-normal so no need to normalize the cross
	// ||up x forward|| = ||up|| ||forward|| sin90 = 1
	R = XMVector3Cross(U, F);

	// Fill in the view matrix entries
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, F));

	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
	XMStoreFloat3(&forward, F);

	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
	XMStoreFloat3(&forward, F);

	XMFLOAT4X4 temp;
	temp(0, 0) = right.x;
	temp(0, 1) = right.y;
	temp(0, 2) = right.z;
	temp(0, 3) = x;
	
	temp(1, 0) = up.x;
	temp(1, 1) = up.y;
	temp(1, 2) = up.z;
	temp(1, 3) = y;
	
	temp(2, 0) = forward.x;
	temp(2, 1) = forward.y;
	temp(2, 2) = forward.z;
	temp(2, 3) = z;
		 
	temp(3, 0) = 0.0f;
	temp(3, 1) = 0.0f;
	temp(3, 2) = 0.0f;
	temp(3, 3) = 1.0f;

	viewMatrix = temp;

	// Recreate view matrix
	//XMMATRIX V = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), GetUpXM());
	//XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
}

#pragma region Accessors

XMVECTOR Camera::GetForwardXM() const
{
	return XMLoadFloat3(&forward);
}

XMFLOAT3 Camera::GetForward() const
{
	return forward;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&up);
}

XMFLOAT3 Camera::GetUp() const
{
	return up;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&right);
}

XMFLOAT3 Camera::GetRight() const
{
	return right;
}

#pragma endregion