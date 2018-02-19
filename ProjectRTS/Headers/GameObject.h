#pragma once

#include <memory>

#include "D3D.h"

class GameObject
{
public:
    explicit GameObject(const D3DClass* direct3D);

	virtual ~GameObject() = default;
	GameObject(const GameObject &) = default;
	GameObject(GameObject &&) = default;
	GameObject &operator=(const GameObject &) = default;
	GameObject &operator=(GameObject &&) = default;

    virtual void render(ID3D12GraphicsCommandList* commandList);
protected:
    std::unique_ptr<ID3D12Resource> m_pVertexBuffer;
    std::unique_ptr<ID3D12Resource> m_pTexture;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    unsigned int m_verticesCount = 0;
    std::unique_ptr<ID3D12DescriptorHeap> m_pSRVHeap;
};