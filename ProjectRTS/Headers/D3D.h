#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>
#include <memory>
#include <functional>
#include <vector>

#include "Pipeline.h"

class D3DClass
{
public:
    struct Float3
    {
        float x;
        float y;
        float z;
    };
    struct Float4
    {
        float x;
        float y;
        float z;
        float w;
    };
    struct Vertex
    {
        Float3 position;
        Float4 color;
    };

    D3DClass(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen);
    ~D3DClass();

    std::shared_ptr<ID3D12Resource> createBufferFromData(unsigned char* data, unsigned long long size);
    std::shared_ptr<ID3D12GraphicsCommandList> createCommandList(std::shared_ptr<Pipeline> pipeline);
    void createRenderTarget(
        unsigned int count, 
        std::shared_ptr<ID3D12DescriptorHeap> &pRenderTargetViewHeap, 
        std::vector<std::shared_ptr<ID3D12Resource>> &renderTargetsArray, 
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &renderTargetViewHandles);
    void createConstantBuffer(
        std::shared_ptr<ID3D12DescriptorHeap> &pConstantBufferViewHeap,
        std::shared_ptr<ID3D12Resource> &pConstantBuffer,
        unsigned int bufferSize,
        D3D12_CPU_DESCRIPTOR_HANDLE &constantBufferViewHandle,
        unsigned char* pData,
        unsigned long long size);

	//void shutdown();

    void executeCommandList(unsigned int count, ID3D12CommandList*const* lists);
    void presentBackBuffer();

    std::shared_ptr<ID3D12Device> getDevice();
    std::shared_ptr<ID3D12RootSignature> getRootSignature();
    std::shared_ptr<ID3D12CommandAllocator> getCommandAllocator();

    unsigned int getBackBufferIndex();
private:

	bool m_vsync_enabled;
	std::shared_ptr<ID3D12Device> m_pDevice;
    std::shared_ptr<ID3D12CommandQueue> m_pCommandQueue;
	char m_videoCardDescription[128];
    std::shared_ptr<IDXGISwapChain3> m_pSwapChain;
	unsigned int m_backBufferIndex;
    unsigned int m_backBuffersCount;
	unsigned int m_videoCardMemory;
    std::shared_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
    std::shared_ptr<ID3D12Fence> m_pFence;
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue;
    std::shared_ptr<ID3D12RootSignature> m_pRootSignature;
};
