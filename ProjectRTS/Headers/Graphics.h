#pragma once

#include <windows.h>
#include <memory>
#include <DirectXMath.h>

#include "D3D.h"
#include "Camera.h"
#include "GameObject.h"

class GraphicsClass
{
public:
	GraphicsClass(int screenHeight, int screenWidth, HWND hwnd);
    ~GraphicsClass();
	bool render();

    Camera* getCamera();
    ID3D12DescriptorHeap* getSRVHeap();
    void updateVertexConstantBuffer();

    static const bool FULL_SCREEN = false;
    static const bool VSYNC_ENABLED = true;
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;

private:
    void GraphicsClass::recordCommandList();

    std::unique_ptr<D3DClass> m_pDirect3D;

    std::unique_ptr<Pipeline> m_pPipeline;
    std::unique_ptr<ID3D12DescriptorHeap> m_pRenderTargetViewHeap;
    std::vector<std::unique_ptr<ID3D12Resource>> m_backBufferRenderTargets;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_renderTargetViewHandles;
    std::unique_ptr<ID3D12GraphicsCommandList> m_pCommandList;
    std::unique_ptr<ID3D12Resource>  m_pVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    std::vector<std::unique_ptr<GameObject>> m_gameObjects;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    std::unique_ptr<Camera> m_pCamera;
    DirectX::XMMATRIX m_worldMatrix;

    struct SVertexConstantBuffer
    {
        DirectX::XMMATRIX m_vpMatrix;
        DirectX::XMMATRIX m_worldMatrix;
    };
    std::unique_ptr<ID3D12Resource>  m_pVertexConstantBuffer;
    std::unique_ptr<ID3D12DescriptorHeap> m_pConstantBufferViewHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_constantBufferViewHandle;
};