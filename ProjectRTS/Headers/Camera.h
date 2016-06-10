#pragma once

#include <DirectXMath.h>

class Camera
{
public:
    Camera(DirectX::XMFLOAT3 position, float screenWidth, float screenHeight);

    void setPosition(DirectX::XMFLOAT3 newPosition);
    void moveCamera(DirectX::XMFLOAT3 offset);
    DirectX::XMMATRIX getVPMatrix();
private:
    void recalculateView();

    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;

    DirectX::XMVECTOR m_cameraPosition;
    DirectX::XMVECTOR m_cameraTarget;
    DirectX::XMVECTOR m_upVector;
};