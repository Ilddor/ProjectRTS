#include "Pipeline.h"
#include "d3dx12.h"
#include "d3dcompiler.h"

Pipeline::Pipeline() :
    m_pPipelineState( nullptr )
{
    m_pipelineDesc = {};
    m_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT_WIREFRAME);
    m_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    m_pipelineDesc.DepthStencilState.DepthEnable = false;
    m_pipelineDesc.DepthStencilState.StencilEnable = false;
    m_pipelineDesc.SampleMask = UINT_MAX;
    m_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    m_pipelineDesc.NumRenderTargets = 1;
    m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    m_pipelineDesc.SampleDesc.Count = 1;
}

Pipeline::~Pipeline()
{
    m_pPipelineState->Release();
}

void Pipeline::createPipeline(std::shared_ptr<ID3D12Device> device)
{
    if (m_pPipelineState != nullptr)
        m_pPipelineState->Release();

    ID3D12PipelineState* state = nullptr;
    HRESULT result = device->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&state));
    m_pPipelineState = std::shared_ptr<ID3D12PipelineState>(state);
}

void Pipeline::setInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
    m_pipelineDesc.InputLayout = layout;
}

void Pipeline::loadShader(ShaderType type, std::wstring path)
{
    ID3DBlob* shader;

    D3DReadFileToBlob(path.data(), &shader);
    if(m_shadersMap.find(type) != m_shadersMap.end())
        m_shadersMap.erase(type);

    m_shadersMap.insert(std::pair<ShaderType, std::shared_ptr<ID3DBlob>>(type, std::shared_ptr<ID3DBlob>(shader)));
    
    switch (type)
    {
    case ShaderType::VERTEX_SHADER :
        m_pipelineDesc.VS = { shader->GetBufferPointer(), shader->GetBufferSize() };
        break;
    case ShaderType::HULL_SHADER :
        m_pipelineDesc.HS = { shader->GetBufferPointer(), shader->GetBufferSize() };
        break;
    case ShaderType::DOMAIN_SHADER:
        m_pipelineDesc.DS = { shader->GetBufferPointer(), shader->GetBufferSize() };
        break;
    case ShaderType::PIXEL_SHADER:
        m_pipelineDesc.PS = { shader->GetBufferPointer(), shader->GetBufferSize() };
        break;
    }
}

void Pipeline::setRootSignature(std::shared_ptr<ID3D12RootSignature> rootSignature)
{
    m_pipelineDesc.pRootSignature = rootSignature.get();
}

std::shared_ptr<ID3D12PipelineState> Pipeline::getPipelineState()
{
    return m_pPipelineState;
}
