#pragma once

#include <d3d12.h>
#include <memory>
#include <map>

class Pipeline
{
public:
    enum ShaderType
    {
        VERTEX_SHADER,
        HULL_SHADER,
        DOMAIN_SHADER,
        PIXEL_SHADER
    };

    Pipeline();
    ~Pipeline();

    void createPipeline(ID3D12Device* device);
    void setInputLayout(D3D12_INPUT_LAYOUT_DESC layout);
    void loadShader(ShaderType type, std::wstring path);
    void setRootSignature(ID3D12RootSignature* rootSignature);

    ID3D12PipelineState* getPipelineState() const;

private:

    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineDesc;
    std::unique_ptr<ID3D12PipelineState> m_pPipelineState;
    std::map<ShaderType, std::shared_ptr<ID3DBlob>> m_shadersMap;
};