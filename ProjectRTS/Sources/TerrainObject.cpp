#include "TerrainObject.h"

#include <array>
#include "d3dx12.h"

TerrainObject::TerrainObject(std::shared_ptr<D3DClass> direct3D, std::shared_ptr<ID3D12DescriptorHeap> pSRVHeap) :
    GameObject(direct3D)
{
    // Create vertex buffer
    std::array<D3DClass::Vertex, 6> terrainVertices =
	//D3DClass::Vertex terrainVertices[6] =
	{ {
		//{ { -1.f, 0.f, 1.f },{ 1.f, 0.f, 0.f, 1.f } },
		//{ { 1.f, 0.f, 1.f },{ 0.f, 1.f, 0.f, 1.f } },
		//{ { -1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
		//{ { -1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
		//{ { 1.f, 0.f, 1.f },{ 0.f, 1.f, 0.f, 1.f } },
		//{ { 1.f, 0.f, -1.f },{ 0.f, 0.f, 1.f, 1.f } },
		{ { -1.f, 1.f, 0.f },{ 1.f, 0.f, 0.f, 1.f } },
		{ { 1.f, 1.f, 0.f },{ 0.f, 1.f, 0.f, 1.f } },
		{ { -1.f, -1.f, 0.f },{ 0.f, 0.f, 1.f, 1.f } },
		{ { -1.f, -1.f, 0.f },{ 0.f, 0.f, 1.f, 1.f } },
		{ { 1.f, 1.f, 0.f },{ 0.f, 1.f, 0.f, 1.f } },
		{ { 1.f, -1.f, 0.f },{ 0.f, 0.f, 1.f, 1.f } },
	} };

	m_verticesCount = terrainVertices.size();
    const UINT vertexBufferSize = m_verticesCount * sizeof(D3DClass::Vertex);

    m_pVertexBuffer = direct3D->createBufferFromData(reinterpret_cast<unsigned char*>(terrainVertices.data()), vertexBufferSize);

    m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(D3DClass::Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;

    //CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 32, 32, 1, 1);
    //direct3D->createTexture(pSRVHeap, textureDesc, m_pTexture);
}

void TerrainObject::render(std::shared_ptr<ID3D12GraphicsCommandList> commandList)
{
    //commandList->SetGraphicsRootShaderResourceView(0, m_pTexture->GetGPUVirtualAddress());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->DrawInstanced(m_verticesCount, 1, 0, 0);
}