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

	D3DClass();
    D3DClass(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen);

    std::shared_ptr<ID3D12Resource> createBufferFromData(unsigned char* data, unsigned long long size);
    std::shared_ptr<ID3D12GraphicsCommandList> createCommandList(std::shared_ptr<Pipeline> pipeline);

	void shutdown();

	bool render();

    std::shared_ptr<ID3D12Device> getDevice();
    std::shared_ptr<ID3D12RootSignature> getRootSignature();
    std::shared_ptr<ID3D12CommandAllocator> getCommandAllocator();
private:

	bool m_vsync_enabled;
	std::shared_ptr<ID3D12Device> m_pDevice;
	ID3D12CommandQueue* m_commandQueue;
	char m_videoCardDescription[128];
	IDXGISwapChain3* m_swapChain;
	ID3D12DescriptorHeap* m_renderTargetViewHeap;
	ID3D12Resource* m_backBufferRenderTarget[2];
	unsigned int m_bufferIndex;
	unsigned int m_videoCardMemory;
    std::shared_ptr<ID3D12CommandAllocator> m_pCommandAllocator;
    std::shared_ptr<ID3D12GraphicsCommandList> m_pCommandList;
	//ID3D12PipelineState* m_pPipeline;
    std::shared_ptr<Pipeline> m_pPipeline;
	ID3D12Fence* m_fence;
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue;
    ID3DBlob* m_vertexShader;
    ID3DBlob* m_pixelShader;
    ID3DBlob* m_hullShader;
    ID3DBlob* m_domainShader;
    std::shared_ptr<ID3D12RootSignature> m_pRootSignature;
    std::shared_ptr<ID3D12Resource>  m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
};
