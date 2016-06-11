#pragma once

#include <memory>

#include "D3D.h"

class GameObject
{
public:
    GameObject(std::shared_ptr<D3DClass> direct3D);

    virtual void render(std::shared_ptr<ID3D12GraphicsCommandList> commandList);
protected:
    std::shared_ptr<ID3D12Resource>  m_pVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    unsigned int m_verticesCount;
};