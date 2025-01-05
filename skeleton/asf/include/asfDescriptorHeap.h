//-----------------------------------------------------------------------------
// File : asfDescriptorHeap.h
// Desc : Descriptor Heap.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <d3d12.h>
#include <asfOffsetAllocator.h>


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// DescriptorHeap class
///////////////////////////////////////////////////////////////////////////////
class DescriptorHeap
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================
    bool Init(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc);
    void Term();

    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU(const OffsetHandle& handle);
    D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU(const OffsetHandle& handle);
    ID3D12DescriptorHeap* GetD3D12DescriptorHeap() const;

    OffsetHandle Alloc(uint32_t count);
    void Free(OffsetHandle& handle);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    ID3D12DescriptorHeap*   m_pHeap     = nullptr;
    uint32_t                m_Increment = 0;
    OffsetAllocator         m_Allocator;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};

} // namespace asf
