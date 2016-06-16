#include "TerrainObject.h"

#include <array>

TerrainObject::TerrainObject(std::shared_ptr<D3DClass> direct3D) :
    GameObject(direct3D)
{
    // Create vertex buffer
    std::array<D3DClass::Vertex, 6> terrainVertices =
    { {
        { { -1.f, 0.f, 1.f },{ 1.f, 0.f, 0.f, 1.f } },
        { { 1.f, 0.f, 1.f },{ 0.f, 1.f, 0.f, 1.f } },
        { { -1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
        { { -1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
        { { 1.f, 0.f, 1.f },{ 0.f, 1.f, 0.f, 1.f } },
        { { 1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
    } };

    m_verticesCount = terrainVertices.size();
    const UINT vertexBufferSize = m_verticesCount * sizeof(D3DClass::Vertex);

    m_pVertexBuffer = direct3D->createBufferFromData(reinterpret_cast<unsigned char*>(terrainVertices.data()), vertexBufferSize);

    m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(D3DClass::Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
}

void TerrainObject::render(std::shared_ptr<ID3D12GraphicsCommandList> commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->DrawInstanced(m_verticesCount, 1, 0, 0);
}