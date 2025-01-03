//-----------------------------------------------------------------------------
// File : TargetView.h
// Desc : Target View
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DescriptorHeap.h>


///////////////////////////////////////////////////////////////////////////////
// TargetDesc structure
///////////////////////////////////////////////////////////////////////////////
struct TargetDesc
{
    D3D12_RESOURCE_DIMENSION    Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    uint64_t                    Alignment           = 0;
    uint64_t                    Width               = 0;
    uint32_t                    Height              = 1;
    uint16_t                    DepthOrArraySize    = 1;
    uint16_t                    MipLevels           = 1;
    DXGI_FORMAT                 Format              = DXGI_FORMAT_UNKNOWN;
    DXGI_SAMPLE_DESC            SampleDesc          = { 1, 0 };
    D3D12_RESOURCE_STATES       InitState           = D3D12_RESOURCE_STATE_COMMON;
    float                       ClearColor[4]       = { 1.0f, 1.0f, 1.0f, 1.0f };
    float                       ClearDepth          = 1.0f;
    uint8_t                     ClearStencil        = 0;
};

///////////////////////////////////////////////////////////////////////////////
// ColorTarget class
///////////////////////////////////////////////////////////////////////////////
class ColorTarget
{
public:
    bool Init(ID3D12Device* pDevice, DescriptorHeap* pHeapRTV, const TargetDesc& desc);
    bool Init(ID3D12Device* pDevice, DescriptorHeap* pHeapRTV, IDXGISwapChain* pSwapChain, uint32_t backBufferIndex);
    void Term();
    ID3D12Resource* GetD3D12Resource() const;
    const TargetDesc& GetDesc() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

private:
    TargetDesc          m_Desc      = {};
    ID3D12Resource*     m_pResource = nullptr;
    DescriptorHeap*     m_pHeap     = nullptr;
    OffsetHandle        m_HandleRTV = {};
};

///////////////////////////////////////////////////////////////////////////////
// DepthTarget class
///////////////////////////////////////////////////////////////////////////////
class DepthTarget
{
public:
    bool Init(ID3D12Device* pDevice, DescriptorHeap* pHeapDSV, const TargetDesc& desc);
    void Term();
    ID3D12Resource* GetD3D12Resource() const;
    const TargetDesc& GetDesc() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

private:
    TargetDesc      m_Desc      = {};
    ID3D12Resource* m_pResource = nullptr;
    DescriptorHeap* m_pHeap     = nullptr;
    OffsetHandle    m_HandleDSV = {};
};
