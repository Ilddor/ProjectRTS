#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_4.h>

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
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, HWND, bool, bool);
	void Shutdown();

	bool Render();

private:

	bool m_vsync_enabled;
	ID3D12Device* m_device;
	ID3D12CommandQueue* m_commandQueue;
	char m_videoCardDescription[128];
	IDXGISwapChain3* m_swapChain;
	ID3D12DescriptorHeap* m_renderTargetViewHeap;
	ID3D12Resource* m_backBufferRenderTarget[2];
	unsigned int m_bufferIndex;
	unsigned int m_videoCardMemory;
	ID3D12CommandAllocator* m_commandAllocator;
	ID3D12GraphicsCommandList* m_commandList;
	ID3D12PipelineState* m_pipelineState;
	ID3D12Fence* m_fence;
	HANDLE m_fenceEvent;
	unsigned long long m_fenceValue;
    ID3DBlob* m_vertexShader;
    ID3DBlob* m_pixelShader;
    ID3DBlob* m_hullShader;
    ID3DBlob* m_domainShader;
    ID3D12RootSignature* m_rootSignature;
    ID3D12Resource*  m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
};
