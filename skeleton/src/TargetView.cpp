//-----------------------------------------------------------------------------
// File : TargetView.cpp
// Desc : Target View.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <TargetView.h>


///////////////////////////////////////////////////////////////////////////////
// ColorTarget class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool ColorTarget::Init(ID3D12Device* pDevice, DescriptorHeap* pHeapRTV, const TargetDesc& targetDesc)
{
    if (pDevice == nullptr || pHeapRTV == nullptr)
    { return false; }

    if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    { return false; }

    {
        D3D12_HEAP_PROPERTIES props = {
            D3D12_HEAP_TYPE_DEFAULT,
            D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            D3D12_MEMORY_POOL_UNKNOWN,
            1,
            1
        };

        D3D12_RESOURCE_DESC desc = {
            targetDesc.Dimension,
            targetDesc.Alignment,
            targetDesc.Width,
            targetDesc.Height,
            targetDesc.DepthOrArraySize,
            targetDesc.MipLevels,
            targetDesc.Format,
            targetDesc.SampleDesc,
            D3D12_TEXTURE_LAYOUT_UNKNOWN,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
        };

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = targetDesc.Format;
        clearValue.Color[0] = targetDesc.ClearColor[0];
        clearValue.Color[1] = targetDesc.ClearColor[1];
        clearValue.Color[2] = targetDesc.ClearColor[2];
        clearValue.Color[3] = targetDesc.ClearColor[3];

        auto hr = pDevice->CreateCommittedResource(
            &props,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            targetDesc.InitState,
            &clearValue,
            IID_PPV_ARGS(&m_pResource));
        if (FAILED(hr))
        { return false; }
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = targetDesc.Format;

    if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
    {
        if (targetDesc.DepthOrArraySize > 1)
        {
            rtvDesc.ViewDimension                   = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.ArraySize        = targetDesc.DepthOrArraySize;
            rtvDesc.Texture1DArray.FirstArraySlice  = 0;
            rtvDesc.Texture1DArray.MipSlice         = 0;
        }
        else
        {
            rtvDesc.ViewDimension       = D3D12_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice  = 0;
        }
    }
    else if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
    {
        if (targetDesc.DepthOrArraySize > 1)
        {
            if (targetDesc.SampleDesc.Count > 1)
            {
                rtvDesc.ViewDimension                       = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                rtvDesc.Texture2DMSArray.ArraySize          = targetDesc.DepthOrArraySize;
                rtvDesc.Texture2DMSArray.FirstArraySlice    = 0;
            }
            else
            {
                rtvDesc.ViewDimension                   = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                rtvDesc.Texture2DArray.ArraySize        = targetDesc.DepthOrArraySize;
                rtvDesc.Texture2DArray.FirstArraySlice  = 0;
                rtvDesc.Texture2DArray.MipSlice         = 0;
                rtvDesc.Texture2DArray.PlaneSlice       = 0;
            }
        }
        else
        {
            if (targetDesc.SampleDesc.Count > 1)
            {
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                rtvDesc.ViewDimension           = D3D12_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice      = 0;
                rtvDesc.Texture2D.PlaneSlice    = 0;
            }
        }
    }
    else if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
    {
        rtvDesc.ViewDimension           = D3D12_RTV_DIMENSION_TEXTURE3D;
        rtvDesc.Texture3D.FirstWSlice   = 0;
        rtvDesc.Texture3D.MipSlice      = 0;
        rtvDesc.Texture3D.WSize         = targetDesc.DepthOrArraySize;
    }

    m_HandleRTV = pHeapRTV->Alloc(1);
    m_pHeap     = pHeapRTV;

    auto handle = pHeapRTV->GetHandleCPU(m_HandleRTV);

    pDevice->CreateRenderTargetView(m_pResource, &rtvDesc, handle);

    return true;
}

//-----------------------------------------------------------------------------
//      スワップチェインから初期化処理を行います.
//-----------------------------------------------------------------------------
bool ColorTarget::Init(ID3D12Device* pDevice, DescriptorHeap* pHeapRTV, IDXGISwapChain* pSwapChain, uint32_t backBufferIndex)
{
    if (pDevice == nullptr || pHeapRTV == nullptr || pSwapChain == nullptr)
    { return false; }

    auto hr = pSwapChain->GetBuffer(backBufferIndex, IID_PPV_ARGS(&m_pResource));
    if (FAILED(hr))
    { return false; }

    auto desc = m_pResource->GetDesc();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = desc.Format;

    if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
    {
        if (desc.DepthOrArraySize > 1)
        {
            rtvDesc.ViewDimension                   = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.ArraySize        = desc.DepthOrArraySize;
            rtvDesc.Texture1DArray.FirstArraySlice  = 0;
            rtvDesc.Texture1DArray.MipSlice         = 0;
        }
        else
        {
            rtvDesc.ViewDimension       = D3D12_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice  = 0;
        }
    }
    else if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
    {
        if (desc.DepthOrArraySize > 1)
        {
            if (desc.SampleDesc.Count > 1)
            {
                rtvDesc.ViewDimension                       = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
                rtvDesc.Texture2DMSArray.ArraySize          = desc.DepthOrArraySize;
                rtvDesc.Texture2DMSArray.FirstArraySlice    = 0;
            }
            else
            {
                rtvDesc.ViewDimension                   = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                rtvDesc.Texture2DArray.ArraySize        = desc.DepthOrArraySize;
                rtvDesc.Texture2DArray.FirstArraySlice  = 0;
                rtvDesc.Texture2DArray.MipSlice         = 0;
                rtvDesc.Texture2DArray.PlaneSlice       = 0;
            }
        }
        else
        {
            if (desc.SampleDesc.Count > 1)
            {
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                rtvDesc.ViewDimension           = D3D12_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice      = 0;
                rtvDesc.Texture2D.PlaneSlice    = 0;
            }
        }
    }
    else if (desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
    {
        rtvDesc.ViewDimension           = D3D12_RTV_DIMENSION_TEXTURE3D;
        rtvDesc.Texture3D.FirstWSlice   = 0;
        rtvDesc.Texture3D.MipSlice      = 0;
        rtvDesc.Texture3D.WSize         = desc.DepthOrArraySize;
    }

    m_HandleRTV = pHeapRTV->Alloc(1);
    m_pHeap     = pHeapRTV;

    auto handle = pHeapRTV->GetHandleCPU(m_HandleRTV);

    pDevice->CreateRenderTargetView(m_pResource, &rtvDesc, handle);

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void ColorTarget::Term()
{
    if (m_pHeap != nullptr)
    {
        m_pHeap->Free(m_HandleRTV);
        m_pHeap = nullptr;
    }

    if (m_pResource != nullptr)
    {
        m_pResource->Release();
        m_pResource = nullptr;
    }

    memset(&m_Desc, 0, sizeof(m_Desc));
}

//-----------------------------------------------------------------------------
//      D3D12リソースを取得します.
//-----------------------------------------------------------------------------
ID3D12Resource* ColorTarget::GetD3D12Resource() const
{ return m_pResource; }

//-----------------------------------------------------------------------------
//      構成設定を取得します.
//-----------------------------------------------------------------------------
const TargetDesc& ColorTarget::GetDesc() const
{ return m_Desc; }

//-----------------------------------------------------------------------------
//      CPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE ColorTarget::GetHandleCPU() const
{
    if (m_pHeap != nullptr)
    { return m_pHeap->GetHandleCPU(m_HandleRTV); }

    return {};
}


///////////////////////////////////////////////////////////////////////////////
// DepthTarget class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool DepthTarget::Init(ID3D12Device* pDevice, DescriptorHeap* pHeapDSV, const TargetDesc& targetDesc)
{
    if (pDevice == nullptr || pHeapDSV == nullptr)
    { return false; }

    if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER
     || targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
    { return false; }

    {
        D3D12_HEAP_PROPERTIES props = {
            D3D12_HEAP_TYPE_DEFAULT,
            D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            D3D12_MEMORY_POOL_UNKNOWN,
            1,
            1
        };

        D3D12_RESOURCE_DESC desc = {
            targetDesc.Dimension,
            targetDesc.Alignment,
            targetDesc.Width,
            targetDesc.Height,
            targetDesc.DepthOrArraySize,
            targetDesc.MipLevels,
            targetDesc.Format,
            targetDesc.SampleDesc,
            D3D12_TEXTURE_LAYOUT_UNKNOWN,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        };

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = targetDesc.Format;
        clearValue.DepthStencil.Depth = targetDesc.ClearDepth;
        clearValue.DepthStencil.Stencil = targetDesc.ClearStencil;

        auto hr = pDevice->CreateCommittedResource(
            &props,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            targetDesc.InitState,
            &clearValue,
            IID_PPV_ARGS(&m_pResource));

        if (FAILED(hr))
        { return false; }
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = targetDesc.Format;
    dsvDesc.Flags  = D3D12_DSV_FLAG_NONE;

    if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
    {
        if (targetDesc.DepthOrArraySize > 1)
        {
            dsvDesc.ViewDimension                   = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            dsvDesc.Texture1DArray.ArraySize        = targetDesc.DepthOrArraySize;
            dsvDesc.Texture1DArray.FirstArraySlice  = 0;
            dsvDesc.Texture1DArray.MipSlice         = 0;
        }
        else
        {
            dsvDesc.ViewDimension       = D3D12_DSV_DIMENSION_TEXTURE1D;
            dsvDesc.Texture1D.MipSlice  = 0;
        }
    }
    else if (targetDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
    {
        if (targetDesc.DepthOrArraySize > 1)
        {
            if (targetDesc.SampleDesc.Count > 1)
            {
                dsvDesc.ViewDimension                       = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
                dsvDesc.Texture2DMSArray.ArraySize          = targetDesc.DepthOrArraySize;
                dsvDesc.Texture2DMSArray.FirstArraySlice    = 0;
            }
            else
            {
                dsvDesc.ViewDimension                   = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                dsvDesc.Texture2DArray.ArraySize        = targetDesc.DepthOrArraySize;
                dsvDesc.Texture2DArray.FirstArraySlice  = 0;
                dsvDesc.Texture2DArray.MipSlice         = 0;
            }
        }
        else
        {
            if (targetDesc.SampleDesc.Count > 1)
            {
                dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                dsvDesc.ViewDimension       = D3D12_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice  = 0;
            }
        }
    }

    m_HandleDSV = pHeapDSV->Alloc(1);
    m_pHeap     = pHeapDSV;

    auto handle = pHeapDSV->GetHandleCPU(m_HandleDSV);

    pDevice->CreateDepthStencilView(m_pResource, &dsvDesc, handle);

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void DepthTarget::Term()
{
    if (m_pHeap != nullptr)
    {
        m_pHeap->Free(m_HandleDSV);
        m_pHeap = nullptr;
    }

    if (m_pResource != nullptr)
    {
        m_pResource->Release();
        m_pResource = nullptr;
    }

    memset(&m_Desc, 0, sizeof(m_Desc));
}

//-----------------------------------------------------------------------------
//      D3D12リソースを取得します.
//-----------------------------------------------------------------------------
ID3D12Resource* DepthTarget::GetD3D12Resource() const
{ return m_pResource; }

//-----------------------------------------------------------------------------
//      構成設定を取得します.
//-----------------------------------------------------------------------------
const TargetDesc& DepthTarget::GetDesc() const
{ return m_Desc; }

//-----------------------------------------------------------------------------
//      CPUディスクリプタハンドルを取得します.
//-----------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE DepthTarget::GetHandleCPU() const
{
    if (m_pHeap != nullptr)
    { return m_pHeap->GetHandleCPU(m_HandleDSV); }

    return {};
}