#include "Camera.h"

#include <iostream>;

using namespace std;

Camera::Camera()
{
	float zPos = -5.0f;
	float zTar = 10.0f;

	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (every frame)
	position = XMVectorSet(0.0f, 0.0f, zPos, 0.0f);
	target = XMVectorSet(0.0f, 0.0f, zTar, 0.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	myPosition = XMFLOAT4(0.0f, 0.0f, zPos, 0);
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

void Camera::Rotate(XMFLOAT3* rotate)
{
	myTarget.x += -1.0f * rotate->x;
	myTarget.y += -1.0f * rotate->y;
	myTarget.z += -1.0f * rotate->z;

	// This is my more complicated attempt
	//XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	//XMVECTOR UP = XMLoadFloat4(&up);
	//XMVECTOR vector = XMVECTOR();
	/*XMFLOAT4 newTarget = myTarget;
	vector = XMLoadFloat4(&newTarget);
	vector = XMVector3Rotate(vector, XMQuaternionRotationMatrix(XMMatrixRotationAxis(UP, -1.0f * rotate->x * XM_PI / 32)));
	vector += target;*/
	//XMStoreFloat4(&myTarget, vector);
	
	// This is probably the better way
	/*XMMATRIX R = XMMatrixRotationY(rotate->x * XM_PI / 8.0f);
	XMFLOAT3 UP = XMFLOAT3();
	XMStoreFloat3(&UP, up);
	XMStoreFloat3(&UP, XMVector3TransformNormal(XMLoadFloat3(&UP), R));
	XMStoreFloat3(&myTarget, XMVector3TransformNormal(XMLoadFloat3(&myTarget), R));*/
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	target = XMVectorSet(myTarget.x, myTarget.y, myTarget.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
}