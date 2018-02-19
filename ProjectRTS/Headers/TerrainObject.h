#pragma once

#include "GameObject.h"

class TerrainObject :
    public GameObject
{
public:
    TerrainObject(D3DClass* direct3D, ID3D12DescriptorHeap* pSRVHeap);

    void render(ID3D12GraphicsCommandList* commandList) override;
};