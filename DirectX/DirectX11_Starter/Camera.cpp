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

	projectionMatrix = XMFLOAT4X4();
}

Camera::~Camera()
{
}

void Camera::Move(XMFLOAT4* move)
{
	myPosition.x += move->x;
	myPosition.y += move->y;
	myPosition.z += move->z;
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
}