#include "Camera.h"

Camera::Camera()
{
    eye = XMVectorSet(0, 0, -1, 1);
    up = XMVectorSet(0, 1, 0, 0);
    at = XMVectorSet(0, 0, 1, 1);

    XMStoreFloat4x4(&move, XMMatrixIdentity());
    XMStoreFloat4x4(&rotate, XMMatrixIdentity());
    XMStoreFloat4x4(&view, XMMatrixLookAtLH(eye, at, up));
}

void Camera::Pitch(float x)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationX(x));
    XMStoreFloat4x4(&rotate, temp); 
}

void Camera::Yaw(float y)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationY(y));
    XMStoreFloat4x4(&rotate, temp);
}

void Camera::Roll(float z)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&rotate), XMMatrixRotationZ(z));
    XMStoreFloat4x4(&rotate, temp);
}

void Camera::MoveForwardBy(float z)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&move), XMMatrixTranslation(0, 0, z));
    XMStoreFloat4x4(&move, temp); 
}

void Camera::MoveRightBy(float x)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&move), XMMatrixTranslation(x, 0, 0));
    XMStoreFloat4x4(&move, temp);
}

void Camera::MoveUpBy(float y)
{
    XMMATRIX temp = XMMatrixMultiply(XMLoadFloat4x4(&move), XMMatrixTranslation(0, y, 0));
    XMStoreFloat4x4(&move, temp);
}

void Camera::Frame()
{
    XMMATRIX temp = XMLoadFloat4x4(&move)*XMLoadFloat4x4(&rotate);
    eye = XMVector4Transform(eye, temp);
    up = XMVector4Transform(up, XMLoadFloat4x4(&rotate));
    at = eye + XMVector4Transform(XMVectorSet(0, 0, 1, 0), XMLoadFloat4x4(&rotate));
    temp = XMMatrixLookAtLH(eye, at, up);
    XMStoreFloat4x4(&view, temp);
    XMStoreFloat4x4(&move, XMMatrixIdentity());
    XMStoreFloat4x4(&rotate, XMMatrixIdentity());
}

void Camera::SetEye(XMVECTOR& e)
{
    e.m128_f32[3] = 1.0f;
    eye = e;
    at = eye + XMVector4Transform(XMVectorSet(0, 0, 1, 0), XMLoadFloat4x4(&rotate));

}

const XMMATRIX Camera::GetViewMatrix()
{
    return XMLoadFloat4x4(&view);
}

XMVECTOR Camera::GetEyePosition()
{
    return eye;
}