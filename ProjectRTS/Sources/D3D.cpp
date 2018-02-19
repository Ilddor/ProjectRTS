#include "D3D.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <array>
#include <fstream>
#include <stdlib.h>
#include <exception>

D3DClass::D3DClass(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen)
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT result;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	IDXGIFactory4* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	UINT stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain* swapChain;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Set the feature level to DirectX 12.1 to enable using all the DirectX 12 features.
	// Note: Not all cards support full DirectX 12, this feature level may need to be reduced on some cards to 12.0.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the Direct3D 12 device.
    ID3D12Device* device;
	result = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&device);
    m_pDevice = std::unique_ptr<ID3D12Device>(device);
	if(FAILED(result))
	{
		throw "Could not create a DirectX 12.1 device.The default video card does not support DirectX 12.1.";
	}

	// Initialize the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	// Set up the description of the command queue.
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	// Create the command queue.
    ID3D12CommandQueue* commandQueue = nullptr;
    m_pDevice->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue);
    m_pCommandQueue = std::unique_ptr<ID3D12CommandQueue>(commandQueue);

	// Create a DirectX graphics interface factory.
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);

	// Use the factory to create an adapter for the primary graphics interface (video card).
	factory->EnumAdapters(0, &adapter);

	// Enumerate the primary adapter output (monitor).
	adapter->EnumOutputs(0, &adapterOutput);


	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];

	// Now fill the display mode list structures.
	adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i = 0; i<numModes; i++)
	{
		if(displayModeList[i].Height == (unsigned int)screenHeight)
		{
			if(displayModeList[i].Width == (unsigned int)screenWidth)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	adapter->GetDesc(&adapterDesc);

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = nullptr;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set the swap chain to use double buffering.
	swapChainDesc.BufferCount = 2;

	// Set the height and width of the back buffers in the swap chain.
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Width = screenWidth;

	// Set a regular 32-bit surface for the back buffers.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the usage of the back buffers to be render target outputs.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the swap effect to discard the previous buffer contents after swapping.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Set to full screen or windowed mode.
    swapChainDesc.Windowed = !fullscreen;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Finally create the swap chain using the swap chain description.	
	factory->CreateSwapChain(m_pCommandQueue.get(), &swapChainDesc, &swapChain);

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
    IDXGISwapChain3* swapChain3 = nullptr;
	swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swapChain3);
    m_pSwapChain = std::unique_ptr<IDXGISwapChain3>(swapChain3);

	// Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	swapChain = nullptr;

	// Release the factory now that the swap chain has been created.
	factory->Release();
	factory = nullptr;

	// Create a command allocator.
    ID3D12CommandAllocator* pCommandAllocator = nullptr;
    m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&pCommandAllocator);
    m_pCommandAllocator = std::unique_ptr<ID3D12CommandAllocator>(pCommandAllocator);

	// Create a fence for GPU synchronization.
    ID3D12Fence* fence = nullptr;
    m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence);
    m_pFence = std::unique_ptr<ID3D12Fence>(fence);

	// Create an event object for the fence.
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);

	// Initialize the starting fence value. 
	m_fenceValue = 1;

    CD3DX12_DESCRIPTOR_RANGE range;
    CD3DX12_ROOT_PARAMETER parameter;

    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(1, &parameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	//rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ID3DBlob* signature;
    ID3DBlob* error;

	result = D3D12SerializeRootSignature(
        &rootSignatureDesc, 
        D3D_ROOT_SIGNATURE_VERSION_1, 
        &signature, 
        &error);
	if (FAILED(result))
	{
		throw "Could not serialize root signature.";
	}

    ID3D12RootSignature* rootSignature;

	result = m_pDevice->CreateRootSignature(
        0, 
        signature->GetBufferPointer(), 
        signature->GetBufferSize(), 
        IID_PPV_ARGS(&rootSignature));
	if (FAILED(result))
	{
		throw "Could not create root signature.";
	}

    m_pRootSignature = std::unique_ptr<ID3D12RootSignature>(rootSignature);

    unsigned long long fenceToWaitFor = m_fenceValue;
    m_pCommandQueue->Signal(m_pFence.get(), fenceToWaitFor);
    m_fenceValue++;

    // Wait until the GPU is done rendering.
    if(m_pFence->GetCompletedValue() < fenceToWaitFor)
    {
        m_pFence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

	m_backBufferIndex = 0;
	m_backBuffersCount = 0;
}

D3DClass::~D3DClass()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if(m_pSwapChain)
    {
        m_pSwapChain->SetFullscreenState(false, NULL);
    }

    // Close the object handle to the fence event.
    CloseHandle(m_fenceEvent);

    m_pFence->Release();
    m_pCommandAllocator->Release();
    m_pSwapChain->Release();
    m_pCommandQueue->Release();
    m_pRootSignature->Release();
    m_pDevice->Release();
}

std::unique_ptr<ID3D12Resource> D3DClass::createBufferFromData(const unsigned char* pData, unsigned long long size)
{
    ID3D12Resource* pBuffer = nullptr;

    m_pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pBuffer));

    unsigned char* pDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
    memcpy(pDataBegin, pData, size);
    pBuffer->Unmap(0, nullptr);

    return std::unique_ptr<ID3D12Resource>(pBuffer);
}

std::unique_ptr<ID3D12GraphicsCommandList> D3DClass::createCommandList(const Pipeline* pPipeline)
{
    ID3D12GraphicsCommandList* pCommandList = nullptr;

    m_pDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_pCommandAllocator.get(),
        pPipeline->getPipelineState(),
        IID_PPV_ARGS(&pCommandList));

    return std::unique_ptr<ID3D12GraphicsCommandList>(pCommandList);
}

std::unique_ptr<ID3D12DescriptorHeap> D3DClass::createRenderTarget(
    unsigned int                                    count, 
    std::vector<std::unique_ptr<ID3D12Resource>>    &renderTargetsArray,
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>        &renderTargetViewHandles)
{
    D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
    unsigned int renderTargetViewDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);;

    m_backBuffersCount = count;
    // Set the number of descriptors to two for our two back buffers.  Also set the heap tyupe to render target views.
    renderTargetViewHeapDesc.NumDescriptors = 2;
    renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // Create the render target view heap for the back buffers.
    ID3D12DescriptorHeap* renderTargetViewHeap = nullptr;
    m_pDevice->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&renderTargetViewHeap);

    // Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
    renderTargetViewHandle = renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
    ID3D12Resource* renderTarget = nullptr;

    for(unsigned int i = 0; i < count; ++i)
    {
        // Get a pointer to the first back buffer from the swap chain.
        m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&renderTarget);

        // Create a render target view for the first back buffer.
        m_pDevice->CreateRenderTargetView(renderTarget, NULL, renderTargetViewHandle);
        renderTargetsArray.push_back(std::unique_ptr<ID3D12Resource>(renderTarget));
        renderTargetViewHandles.push_back(renderTargetViewHandle);

        renderTarget = nullptr;
        // Increment the view handle to the next descriptor location in the render target view heap.
        renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
    }

    // Finally get the initial index to which buffer is the current back buffer.
    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	return std::unique_ptr<ID3D12DescriptorHeap>(renderTargetViewHeap);
}

D3DClass::D3D12ConstantBuffer D3DClass::createConstantBuffer(
    unsigned int bufferSize,
    unsigned char* pData)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ID3D12Resource* pBuffer = nullptr;

    m_pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pBuffer));

    ID3D12DescriptorHeap* constantBufferHeap = nullptr;
    HRESULT result = m_pDevice->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), (void**)(&constantBufferHeap));
	if (FAILED(result))
	{
		throw "Could not create description heap for constant buffer";
	}
    CD3DX12_DESCRIPTOR_RANGE range;


    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = pBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = (bufferSize + 255) & ~255;

	m_pDevice->CreateConstantBufferView(&cbvDesc, constantBufferHeap->GetCPUDescriptorHandleForHeapStart());

    unsigned char* pDataBegin;
    CD3DX12_RANGE readRange(0, 0);
	pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
    memcpy(pDataBegin, pData, bufferSize);
	pBuffer->Unmap(0, nullptr);

	return D3DClass::D3D12ConstantBuffer(std::unique_ptr<ID3D12DescriptorHeap>(constantBufferHeap), std::unique_ptr<ID3D12Resource>(pBuffer));
}

void D3DClass::createTexture(
    std::unique_ptr<ID3D12DescriptorHeap> &pSRVHeap,
    CD3DX12_RESOURCE_DESC &texDesc,
    std::unique_ptr<ID3D12Resource> &texture)
{
    ID3D12Resource* pTexture = nullptr;

    m_pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&pTexture));
    texture = std::unique_ptr<ID3D12Resource>(pTexture);

    /*D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    /*D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ID3D12DescriptorHeap* srvHeap = nullptr;
    m_pDevice->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), (void**)(&srvHeap));
    pSRVHeap = std::shared_ptr<ID3D12DescriptorHeap>(srvHeap);*/

    /*CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(pSRVHeap->GetCPUDescriptorHandleForHeapStart(), m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

    m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, cpuHandle);

    unsigned char* pDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    texture->Map(0, &readRange, reinterpret_cast<void**>(&pDataBegin));
    //memcpy(pDataBegin, pData, size);
    texture->Unmap(0, nullptr);*/
}

void D3DClass::executeCommandList(unsigned int count, ID3D12CommandList*const* lists)
{
    m_pCommandQueue->ExecuteCommandLists(count, lists);
}

void D3DClass::presentBackBuffer()
{
    unsigned long long fenceToWaitFor;

    m_pSwapChain->Present(m_vsync_enabled, 0);

    // Signal and increment the fence value.
    fenceToWaitFor = m_fenceValue;
    m_pCommandQueue->Signal(m_pFence.get(), fenceToWaitFor);
    m_fenceValue++;

    // Wait until the GPU is done rendering.
    if(m_pFence->GetCompletedValue() < fenceToWaitFor)
    {
        m_pFence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    // Alternate the back buffer index back and forth between 0 and 1 each frame.
    m_backBufferIndex += 1;
    if(m_backBufferIndex == m_backBuffersCount)
        m_backBufferIndex = 0;
}

ID3D12Device* D3DClass::getDevice()
{
    return m_pDevice.get();
}

ID3D12RootSignature* D3DClass::getRootSignature()
{
    return m_pRootSignature.get();
}

ID3D12CommandAllocator* D3DClass::getCommandAllocator()
{
    return m_pCommandAllocator.get();
}

unsigned int D3DClass::getBackBufferIndex()
{
    return m_backBufferIndex;
}
