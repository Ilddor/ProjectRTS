#pragma once

#include "GameObject.h"

class TerrainObject :
    public GameObject
{
public:
    TerrainObject(std::shared_ptr<D3DClass> direct3D, std::shared_ptr<ID3D12DescriptorHeap> pSRVHeap);

    virtual void render(std::shared_ptr<ID3D12GraphicsCommandList> commandList);
};