#include "Camera.h"

#include <iostream>;

using namespace std;

Camera::Camera()
{
	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	position = XMVectorSet(0, 0, -5, 0);
	target = XMVectorSet(0, 0, 10, 0);
	up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	myPosition = XMFLOAT4(0, 0, -5, 0);
	myTarget = XMFLOAT4(0, 0, 10, 0);

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
	XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR UP = XMLoadFloat4(&up);

	myTarget.x += -1.0f * rotate->x / 5.0f;
	myTarget.y += -1.0f * rotate->y / 5.0f;
	myTarget.z += -1.0f * rotate->z / 5.0f;

	XMVECTOR vector = XMVECTOR();

	/*XMFLOAT4 newTarget = myTarget;
	vector = XMLoadFloat4(&newTarget);
	vector = XMVector3Rotate(vector, XMQuaternionRotationMatrix(XMMatrixRotationAxis(UP, -1.0f * rotate->x * XM_PI / 32)));
	vector += target;*/

	//XMStoreFloat4(&myTarget, vector);
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	target = XMVectorSet(myTarget.x, myTarget.y, myTarget.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
	//XMMatrixPerspectiveFovLH
	//	XMMatrixPerspectiveFovLH
}