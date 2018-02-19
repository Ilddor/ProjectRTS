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
#include "d3dx12.h"

class D3DClass
{
public:
	using D3D12ConstantBuffer = std::tuple<
		std::unique_ptr<ID3D12DescriptorHeap>,
		std::unique_ptr<ID3D12Resource>>;
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

    std::unique_ptr<ID3D12Resource> createBufferFromData(const unsigned char* data, unsigned long long size);
    std::unique_ptr<ID3D12GraphicsCommandList> createCommandList(const Pipeline* pipeline);
    std::unique_ptr<ID3D12DescriptorHeap>  createRenderTarget(
        unsigned int count,
        std::vector<std::unique_ptr<ID3D12Resource>> &renderTargetsArray,
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> &renderTargetViewHandles);
	D3D12ConstantBuffer createConstantBuffer(
        unsigned int bufferSize,
        unsigned char* pData);
    void createTexture(
        std::unique_ptr<ID3D12DescriptorHeap> &pSRVHeap,
        CD3DX12_RESOURCE_DESC &texDesc,
        std::unique_ptr<ID3D12Resource> &texture);

	//void shutdown();

    void executeCommandList(unsigned int count, ID3D12CommandList*const* lists);
    void presentBackBuffer();

    ID3D12Device* getDevice();
    ID3D12RootSignature* getRootSignature();
    ID3D12CommandAllocator* getCommandAllocator();

    unsigned int getBackBufferIndex();
private:

	bool m_vsync_enabled;
	std::unique_ptr<ID3D12Device> m_pDevice;
    std::unique_ptr<ID3D12CommandQueue> m_pCommandQueue;
	char m_videoCardDescription[128];
    std::unique_ptr<IDXGISwapChain3> m_pSwapChain;
	unsigned int m_backBufferIndex;
    unsigned int m_backBuffersCount;
	unsigned int m_videoCardMemory;
    std::unique_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
    std::unique_ptr<ID3D12Fence> m_pFence;
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue;
    std::unique_ptr<ID3D12RootSignature> m_pRootSignature;
};
