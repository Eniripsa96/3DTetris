#include "Camera.h"

#include <iostream>;

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
	position = XMVectorSet(xPos, yPos, zPos, 0.0f);
	target = XMVectorSet(xTar, yTar, zTar, 0.0f);
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	myPosition = XMFLOAT3(xPos, yPos, zPos);
	myTarget = XMFLOAT3(xTar, yTar, zTar);

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
	position += move * forward() / 100.0f;
	target += move * forward() / 100.0f;
	//XMStoreFloat3(&myPosition, position);
}

void Camera::Rotate(XMFLOAT3* rotate)
{
	XMMATRIX R = XMMatrixRotationY(-1.0f * rotate->x);
	XMFLOAT3 UP = XMFLOAT3();
	XMStoreFloat3(&UP, up);
	XMStoreFloat3(&UP, XMVector3TransformNormal(XMLoadFloat3(&UP), R));
	//XMStoreFloat3(&myTarget, XMVector3TransformNormal(XMLoadFloat3(&myPosition), R));
	XMStoreFloat3(&myTarget, XMVector3TransformNormal(XMLoadFloat3(&myTarget), R));
}

void Camera::Update(float dt)
{
	position = XMVectorSet(myPosition.x, myPosition.y, myPosition.z, 0.0f);
	target = XMVectorSet(myTarget.x, myTarget.y, myTarget.z, 0.0f);
	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));
}

XMVECTOR Camera::forward()
{
	//return XMVector4Normalize(XMLoadFloat3(&myTarget) - XMLoadFloat3(&myPosition));
	//XMFLOAT3 temp;
	//XMStoreFloat3(&temp, XMVector4Normalize(target - position));
	//return temp;
	return XMVector4Normalize(target - position);
}