#include "Camera.h"

#include <iostream>

using namespace std;

Camera::Camera()
{
	float zPos = -5.0f;
	float zTar = 10.0f;

	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	position = XMVectorSet(0.0f, 0.0f, zPos, 0.0f);
	target = XMVectorSet(0.0f, 0.0f, zTar, 0.0f);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, GetUpXM());
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	myPosition = XMFLOAT3(0.0f, 0.0f, zPos);
	myTarget = XMFLOAT3(0.0f, 0.0f, zTar);

	projectionMatrix = XMFLOAT4X4();
}

Camera::~Camera()
{
}

void Camera::Move(XMFLOAT3* move)
{
	myPosition.x += move->x / 10.0f;
	myPosition.y += move->y / 10.0f;
	myPosition.z += move->z / 10.0f;

	myTarget.x += move->x / 10.0f;
	myTarget.y += move->y / 10.0f;
	myTarget.z += move->z / 10.0f;
}

void Camera::MoveVertical(float move)
{

}
void Camera::MoveHorizontal(float move)
{

}
void Camera::MoveDepth(float move)
{
	//myPosition = myPosition + forward();
	//position += move * forward() / 100.0f;
	//target += move * forward() / 100.0f;
	//XMStoreFloat3(&myPosition, position);
}

void Camera::Rotate(XMFLOAT3* rotate)
{
	// Get rotation about Y axis
	XMMATRIX R = XMMatrixRotationY(-1.0f * rotate->x);

	// Rotate basis vectors about Y axis
	XMStoreFloat3(&forward, XMVector3TransformNormal(XMLoadFloat3(&forward), R));
	XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
	XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));

	// Rotate target about Y axis for LookAt function
	XMStoreFloat3(&myTarget, XMVector3TransformNormal(XMLoadFloat3(&myTarget), R));
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	target = XMVectorSet(myTarget.x, myTarget.y, myTarget.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, GetUpXM());
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
}

#pragma region Accessors

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&up);
}

XMFLOAT3 Camera::GetUp() const
{
	return up;
}

#pragma endregion