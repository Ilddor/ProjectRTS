#include "Camera.h"
#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera(DirectX::XMFLOAT3 position, float screenWidth, float screenHeight)
{
    m_cameraTarget = DirectX::XMLoadFloat3(&position);
    m_angle = 0;
    m_upVector = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(90, ((float)screenHeight) / ((float)screenWidth), 0.1f, 1000.f);

    recalculateView();
}

void Camera::setPosition(DirectX::XMFLOAT3 newPosition)
{
    m_cameraTarget = DirectX::XMLoadFloat3(&newPosition);
    recalculateView();
}

void Camera::moveCamera(DirectX::XMFLOAT3 offsetRelative)
{
    DirectX::XMFLOAT3 offsetAbsolute;
    offsetAbsolute.y = offsetRelative.y;
    offsetAbsolute.x = offsetRelative.x*cos(m_angle*M_PI / 180.f) - offsetRelative.z*sin(m_angle*M_PI / 180.f);
    offsetAbsolute.z = offsetRelative.x*sin(m_angle*M_PI / 180.f) + offsetRelative.z*cos(m_angle*M_PI / 180.f);
    m_cameraTarget = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMLoadFloat3(&offsetAbsolute));
    recalculateView();
}

void Camera::ratateCamera(float angleDegrees)
{
    m_angle += angleDegrees;
    if(m_angle > 360.f)
    {
        m_angle -= 360.f;
    }
    recalculateView();
}

DirectX::XMMATRIX Camera::getVPMatrix()
{
    return m_viewMatrix * m_projectionMatrix;
}

void Camera::recalculateView()
{
    float groundCameraDist = 5.f;
    float cameraHeight = 10.f;
    DirectX::XMVECTOR cameraPosition = DirectX::XMVectorAdd(m_cameraTarget, DirectX::XMVectorSet(sin(m_angle*M_PI/180.f)*groundCameraDist, cameraHeight, cos(m_angle*M_PI/180)*-groundCameraDist, 0.f));
    m_viewMatrix = DirectX::XMMatrixLookAtLH(cameraPosition, m_cameraTarget, m_upVector);
}