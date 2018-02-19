#include "Graphics.h"

#include <d3d12.h>
#include <array>
#include "d3dx12.h"
#include "TerrainObject.h"

GraphicsClass::GraphicsClass(int screenHeight, int screenWidth, HWND hwnd)
	: m_pDirect3D(new D3DClass(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN))
{
    // Init render targets with their view handles and view heap
	m_pRenderTargetViewHeap = m_pDirect3D->createRenderTarget(2, m_backBufferRenderTargets, m_renderTargetViewHandles);

    std::array<D3D12_INPUT_ELEMENT_DESC, 2> inputElementDescs =
    {
        D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        D3D12_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    m_pPipeline = std::make_unique<Pipeline>();
    m_pPipeline->setInputLayout({ inputElementDescs.data() , static_cast<UINT>(inputElementDescs.size()) });
    m_pPipeline->setRootSignature(m_pDirect3D->getRootSignature());
    m_pPipeline->loadShader(Pipeline::VERTEX_SHADER, L"DefaultVS.cso");
    m_pPipeline->loadShader(Pipeline::HULL_SHADER, L"DefaultHS.cso");
    m_pPipeline->loadShader(Pipeline::DOMAIN_SHADER, L"DefaultDS.cso");
    m_pPipeline->loadShader(Pipeline::PIXEL_SHADER, L"DefaultPS.cso");
    m_pPipeline->createPipeline(m_pDirect3D->getDevice());

    // Create a basic command list.
    m_pCommandList = m_pDirect3D->createCommandList(m_pPipeline.get());
    m_pCommandList->Close();

    // Initialize viewport and scissor rect
    m_viewport.Height = screenHeight;
    m_viewport.Width = screenWidth;
    m_viewport.MaxDepth = 1000.f;
    m_viewport.MinDepth = 0.1f;
    m_viewport.TopLeftX = 0.f;
    m_viewport.TopLeftY = 0.f;

    m_scissorRect.left = 0;
    m_scissorRect.right = screenWidth;
    m_scissorRect.top = 0;
    m_scissorRect.bottom = screenHeight;

    m_pCamera = std::make_unique<Camera>(DirectX::XMFLOAT3(0.f, 0.f, 0.f), screenWidth, screenHeight);
    m_worldMatrix = DirectX::XMMatrixIdentity();

    SVertexConstantBuffer buffer;
    buffer.m_vpMatrix = m_pCamera->getVPMatrix();
    buffer.m_worldMatrix = m_worldMatrix;
    auto [constantBufferViewHeap, vertexConstantBuffer] = m_pDirect3D->createConstantBuffer(
		sizeof(buffer),
		reinterpret_cast<unsigned char*>(&buffer));
	m_pConstantBufferViewHeap = std::move(constantBufferViewHeap);
	m_pVertexConstantBuffer = std::move(vertexConstantBuffer);

    m_gameObjects.push_back(std::unique_ptr<GameObject>(new TerrainObject(m_pDirect3D.get(), m_pConstantBufferViewHeap.get())));
}

GraphicsClass::~GraphicsClass()
{
    m_pRenderTargetViewHeap->Release();
    m_pCommandList->Release();
    for(const auto& it : m_backBufferRenderTargets)
    {
        it->Release();
    }

    m_pDirect3D->~D3DClass();
}

void GraphicsClass::recordCommandList()
{
	m_pDirect3D->getCommandAllocator()->Reset();
    m_pCommandList->Reset(m_pDirect3D->getCommandAllocator(), m_pPipeline->getPipelineState());

    m_pCommandList->SetGraphicsRootSignature(m_pDirect3D->getRootSignature());
    m_pCommandList->RSSetViewports(1, &m_viewport);
    m_pCommandList->RSSetScissorRects(1, &m_scissorRect);
    //m_pCommandList->SetGraphicsRootConstantBufferView(0, m_pVertexConstantBuffer->GetGPUVirtualAddress());
    std::array<ID3D12DescriptorHeap*, 1> pDescHeaps = { m_pConstantBufferViewHeap.get() };
    m_pCommandList->SetDescriptorHeaps(1, pDescHeaps.data());
	m_pCommandList->SetGraphicsRootDescriptorTable(0, m_pConstantBufferViewHeap->GetGPUDescriptorHandleForHeapStart());

    // Record commands in the command list now.
    // Start by setting the resource barrier.
    m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTargets[m_pDirect3D->getBackBufferIndex()].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Set the back buffer as the render target.
    m_pCommandList->OMSetRenderTargets(1, &m_renderTargetViewHandles[m_pDirect3D->getBackBufferIndex()], static_cast<BOOL>(false), nullptr);

    // Then set the color to clear the window to.
    float color[4] = { 0.5, 0.5, 0.5, 1.0 };
    m_pCommandList->ClearRenderTargetView(m_renderTargetViewHandles[m_pDirect3D->getBackBufferIndex()], color, 0, nullptr);

    for(const auto& it : m_gameObjects)
    {
        it->render(m_pCommandList.get());
    }

    // Indicate that the back buffer will now be used to present.
    m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTargets[m_pDirect3D->getBackBufferIndex()].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    m_pCommandList->Close();
}


bool GraphicsClass::render()
{
	// Render the graphics scene.
    recordCommandList();
    //std::array<ID3D12CommandList*, 1> pCommandLists = { m_pCommandList.get() };
	ID3D12CommandList* ppCommandLists[1];
	ppCommandLists[0] = m_pCommandList.get();
    m_pDirect3D->executeCommandList(1, ppCommandLists);

    m_pDirect3D->presentBackBuffer();

	return true;
}

Camera* GraphicsClass::getCamera()
{
    return m_pCamera.get();
}

ID3D12DescriptorHeap* GraphicsClass::getSRVHeap()
{
    return m_pConstantBufferViewHeap.get();
}

void GraphicsClass::updateVertexConstantBuffer()
{
    SVertexConstantBuffer buffer;
    buffer.m_vpMatrix = m_pCamera->getVPMatrix();
    buffer.m_worldMatrix = m_worldMatrix;
    unsigned char* pDataBegin;
    CD3DX12_RANGE readRange(0, 0);

    m_pVertexConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
    memcpy(pDataBegin, &buffer, sizeof(buffer));
    m_pVertexConstantBuffer->Unmap(0, nullptr);
}