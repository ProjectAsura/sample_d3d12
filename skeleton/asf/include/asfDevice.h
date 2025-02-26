﻿//-----------------------------------------------------------------------------
// File : asfDevice.h
// Desc : Device.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// ResolutionInfo
///////////////////////////////////////////////////////////////////////////////
struct ResolutionInfo
{
    uint32_t        Width;
    uint32_t        Height;
    DXGI_RATIONAL   RefreshRate;
};

///////////////////////////////////////////////////////////////////////////////
// DeviceDesc structure
///////////////////////////////////////////////////////////////////////////////
struct DeviceDesc
{
    bool EnableDebug            = false;
    bool EnableDRED             = true;
    bool EnablePIX              = false;
    bool EnableBreakOnWarning   = false;
    bool EnableBreakOnError     = true;
};

//-----------------------------------------------------------------------------
//! @brief      デバイスの初期化処理を行います.
//-----------------------------------------------------------------------------
bool DeviceInit(const DeviceDesc& desc);

//-----------------------------------------------------------------------------
//! @brief      デバイスの終了処理を行います.
//-----------------------------------------------------------------------------
void DeviceTerm();

//-----------------------------------------------------------------------------
//! @brief      D3D12デバイスを取得します.
//-----------------------------------------------------------------------------
ID3D12Device8* GetD3D12Device();

//-----------------------------------------------------------------------------
//! @brief      DXGIファクトリを取得します.
//-----------------------------------------------------------------------------
IDXGIFactory7* GetDXGIFactory();

//-----------------------------------------------------------------------------
//! @brief      解像度情報を取得します.
//-----------------------------------------------------------------------------
bool GetResolutionInfo(IDXGIOutput* pOutput, DXGI_FORMAT format, std::vector<ResolutionInfo>& result);

//-----------------------------------------------------------------------------
//! @brief      nullptr を考慮して解放処理を行います.
//-----------------------------------------------------------------------------
template<typename T>
void SafeRelease(T*& ptr)
{
    if (ptr != nullptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}

} // namespace asf
