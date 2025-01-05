//-----------------------------------------------------------------------------
// File : asfDescriptorHeap.cpp
// Desc : Descriptor Heap.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asfDescriptorHeap.h>


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// DescriptorHeap class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool DescriptorHeap::Init(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc)
{
    if (pDevice == nullptr || pDesc == nullptr)
    { return false; }

    auto hr = pDevice->CreateDescriptorHeap(pDesc, IID_PPV_ARGS(&m_pHeap));
    if (FAILED(hr))
    { return false; }

    m_Increment = pDevice->GetDescriptorHandleIncrementSize(pDesc->Type);
    m_Allocator.Init(sizeof(uint32_t) * pDesc->NumDescriptors, pDesc->NumDescriptors);
    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void DescriptorHeap::Term()
{
    m_Allocator.Term();
    if (m_pHeap != nullptr)
    {
        m_pHeap->Release();
        m_pHeap = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      CPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHandleCPU(const OffsetHandle& handle)
{
    if (!handle.IsValid() || m_pHeap == nullptr)
        return {};

    auto result = m_pHeap->GetCPUDescriptorHandleForHeapStart();
    result.ptr += SIZE_T(m_Increment) * handle.GetOffset();
    return result;
}

//-----------------------------------------------------------------------------
//      GPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetHandleGPU(const OffsetHandle& handle)
{
    if (!handle.IsValid() || m_pHeap == nullptr)
        return {};

    auto result = m_pHeap->GetGPUDescriptorHandleForHeapStart();
    result.ptr += UINT64(m_Increment) * handle.GetOffset();
    return result;
}

//-----------------------------------------------------------------------------
//      D3D12ディスクリプタヒープを取得します.
//-----------------------------------------------------------------------------
ID3D12DescriptorHeap* DescriptorHeap::GetD3D12DescriptorHeap() const
{ return m_pHeap; }

//-----------------------------------------------------------------------------
//      オフセットハンドルを確保します.
//-----------------------------------------------------------------------------
OffsetHandle DescriptorHeap::Alloc(uint32_t count)
{ return m_Allocator.Alloc(count); }

//-----------------------------------------------------------------------------
//      オフセットハンドルを解放します.
//-----------------------------------------------------------------------------
void DescriptorHeap::Free(OffsetHandle& handle)
{ m_Allocator.Free(handle); }

} // namespace asf
