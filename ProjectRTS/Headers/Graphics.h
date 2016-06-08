#pragma once

#include <windows.h>
#include <memory>
#include <DirectXMath.h>

#include "D3D.h"

class GraphicsClass
{
public:
	GraphicsClass(int screenHeight, int screenWidth, HWND hwnd);
    ~GraphicsClass();
	bool render();

    static const bool FULL_SCREEN = false;
    static const bool VSYNC_ENABLED = true;
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;

private:
    void GraphicsClass::recordCommandList();

    std::unique_ptr<D3DClass> m_pDirect3D;

    std::shared_ptr<Pipeline> m_pPipeline;
    std::shared_ptr<ID3D12DescriptorHeap> m_pRenderTargetViewHeap;
    std::vector<std::shared_ptr<ID3D12Resource>> m_backBufferRenderTargets;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_renderTargetViewHandles;
    std::shared_ptr<ID3D12GraphicsCommandList> m_pCommandList;
    std::shared_ptr<ID3D12Resource>  m_pVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_worldMatrix;

    struct SVertexConstantBuffer
    {
        DirectX::XMMATRIX m_viewMatrix;
        DirectX::XMMATRIX m_projectionMatrix;
        DirectX::XMMATRIX m_worldMatrix;
    };
    std::shared_ptr<ID3D12Resource>  m_pVertexConstantBuffer;
    std::shared_ptr<ID3D12DescriptorHeap> m_pConstantBufferViewHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_constantBufferViewHandle;
};