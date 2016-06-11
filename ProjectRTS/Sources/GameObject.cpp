#include "GameObject.h"

#include <array>

GameObject::GameObject(std::shared_ptr<D3DClass> direct3D)
{
    // Create vertex buffer
    std::array<D3DClass::Vertex, 3> triangleVertices =
    { {
        { { 0.0f, 0.5f, 0.f },{ 1.f, 0.f, 0.f, 1.f } },
        { { 0.5f, -0.5f, 0.f },{ 0.f, 1.f, 0.f, 1.f } },
        { { -0.5f, -0.5f, 0.f },{ 0.f, 0.f, 1.f, 1.f } },
    } };

    m_verticesCount = triangleVertices.size();
    const UINT vertexBufferSize = m_verticesCount * sizeof(D3DClass::Vertex);

    m_pVertexBuffer = direct3D->createBufferFromData(reinterpret_cast<unsigned char*>(triangleVertices.data()), vertexBufferSize);

    m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(D3DClass::Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
}

void GameObject::render(std::shared_ptr<ID3D12GraphicsCommandList> commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->DrawInstanced(m_verticesCount, 1, 0, 0);
}