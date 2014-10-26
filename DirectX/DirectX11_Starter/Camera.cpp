#include "Camera.h"


Camera::Camera()
{
	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	position = XMVectorSet(0, 0, -5, 0);
	target = XMVectorSet(0, 0, 0, 0);
	up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	myPosition = XMFLOAT4(0, 0, -5, 0);
	myTarget = XMFLOAT4(0, 0, 0, 0);

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

void Camera::Rotate(XMFLOAT3* rotate)
{
	myPosition.x += rotate->x / 10.0f;
	myPosition.y += rotate->y / 10.0f;
	myPosition.z += rotate->z / 10.0f;
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	target = XMVectorSet(myTarget.x, myTarget.y, myTarget.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
	//XMMatrixPerspectiveFovLH
}