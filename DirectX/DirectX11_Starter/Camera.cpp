#include "Camera.h"

#include <iostream>

using namespace std;

Camera::Camera()
{
	float zPos = -5.0f;
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

	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::MoveDepth(float move)
{
	// Translate camera along forward vector
	XMVECTOR s = XMVectorReplicate(move);
	XMVECTOR f = XMLoadFloat3(&forward);
	XMVECTOR p = XMLoadFloat3(&position);

	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, f, p));
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

void Camera::Update(float dt)
{
	// Rebuild view matrix
	//position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	//target = XMVectorSet(target.x, target.y, target.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), GetUpXM());
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	// TODO: Renormalize basis vectors
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