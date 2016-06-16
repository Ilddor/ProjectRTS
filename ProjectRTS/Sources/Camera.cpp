#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3 position, float screenWidth, float screenHeight)
{
    m_cameraTarget = DirectX::XMLoadFloat3(&position);
    m_cameraPosition = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMVectorSet(0.f, 10.f, -10.f, 0.f));
    m_upVector = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(120, ((float)screenHeight) / ((float)screenWidth), 0.1f, 1000.f);

    recalculateView();
}

void Camera::setPosition(DirectX::XMFLOAT3 newPosition)
{
    m_cameraTarget = DirectX::XMLoadFloat3(&newPosition);
    m_cameraPosition = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMVectorSet(0.f, 10.f, -10.f, 0.f));
    recalculateView();
}

void Camera::moveCamera(DirectX::XMFLOAT3 offset)
{
    m_cameraTarget = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMLoadFloat3(&offset));
    m_cameraPosition = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMVectorSet(0.f, 10.f, -10.f, 0.f));
    recalculateView();
}

DirectX::XMMATRIX Camera::getVPMatrix()
{

    return m_viewMatrix * m_projectionMatrix;
}

void Camera::recalculateView()
{
    m_viewMatrix = DirectX::XMMatrixLookAtLH(m_cameraPosition, m_cameraTarget, m_upVector);
}