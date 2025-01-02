//-----------------------------------------------------------------------------
// File : Device.cpp
// Desc : Device.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <Device.h>
#include <strsafe.h>
#include <ShlObj.h>
#include <wrl/client.h>

template<typename T>
using RefPtr = Microsoft::WRL::ComPtr<T>;

namespace {

//-----------------------------------------------------------------------------
// Global Variables.
//-----------------------------------------------------------------------------
IDXGIFactory7*      g_pDXGIFactory  = nullptr;
IDXGIAdapter*       g_pDXGIAdapter  = nullptr;
IDXGIOutput6*       g_pDXGIOutput   = nullptr;
ID3D12Device8*      g_pDevice       = nullptr;
ID3D12InfoQueue*    g_pInfoQueue    = nullptr;
ID3D12Debug3*       g_pDebug        = nullptr;


//-----------------------------------------------------------------------------
//      PIXキャプチャー用のDLLをロードします.
//-----------------------------------------------------------------------------
void LoadPixGpuCpatureDll()
{
    LPWSTR programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

    wchar_t pixSearchPath[MAX_PATH] = {};
    StringCchCopy(pixSearchPath, MAX_PATH, programFilesPath);
    StringCchCat (pixSearchPath, MAX_PATH, L"\\Microsoft PIX\\*");

    WIN32_FIND_DATA findData;
    bool foundPixInstallation = false;
    wchar_t newestVersionFound[MAX_PATH] = {};

    HANDLE hFind = FindFirstFile(pixSearchPath, &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
                 (findData.cFileName[0] != '.'))
            {
                if (!foundPixInstallation || wcscmp(newestVersionFound, findData.cFileName) <= 0)
                {
                    foundPixInstallation = true;
                    StringCchCopy(newestVersionFound, _countof(newestVersionFound), findData.cFileName);
                }
            }
        } 
        while (FindNextFile(hFind, &findData) != 0);
    }

    FindClose(hFind);

    if (!foundPixInstallation)
    {
        return;
    }

    wchar_t dllPath[MAX_PATH] = {};
    StringCchCopy(dllPath, wcslen(pixSearchPath), pixSearchPath);
    StringCchCat(dllPath, MAX_PATH, &newestVersionFound[0]);
    StringCchCat(dllPath, MAX_PATH, L"\\WinPixGpuCapturer.dll");

    if (GetModuleHandleW(L"WinPixGpuCapturer.dll") == 0)
    {
        LoadLibraryW(dllPath);
    }
}

} // namespace

//-----------------------------------------------------------------------------
//      デバイスの初期化処理を行います.
//-----------------------------------------------------------------------------
bool DeviceInit(const DeviceDesc& desc)
{
    // PIXのDLLをロード.
    if (desc.EnablePIX)
    { LoadPixGpuCpatureDll(); }

    // デバッグレイヤーの有効か.
    if (desc.EnableDebug)
    {
        RefPtr<ID3D12Debug> debug;
        auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            hr = debug->QueryInterface(IID_PPV_ARGS(&g_pDebug));
            if (SUCCEEDED(hr))
            { g_pDebug->EnableDebugLayer(); }
        }
    }

    // DREDの有効化.
    if (desc.EnableDRED)
    {
        RefPtr<ID3D12DeviceRemovedExtendedDataSettings1> dred;
        auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(dred.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            dred->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
            dred->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
            dred->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        }
    }

    // DXGIファクトリーを生成.
    {
        uint32_t flags = 0;
        if (desc.EnableDebug)
        { flags |= DXGI_CREATE_FACTORY_DEBUG; }

        RefPtr<IDXGIFactory2> factory;
        auto hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(factory.GetAddressOf()));
        if (FAILED(hr))
        {
            //ELOG("Error : CreateDXGIFactory2() Failed. errcode = 0x%x", hr);
            return false;
        }

        hr = factory->QueryInterface(IID_PPV_ARGS(&g_pDXGIFactory));
        if (FAILED(hr))
        {
            //ELOG("Error : QueryInterface() Failed. errcode = 0x%x", hr);
            return false;
        }
    }

    // DXGIOutputとデバイス生成.
    {
        RefPtr<IDXGIAdapter1> adapter;
        for(auto adapterId=0u;
            DXGI_ERROR_NOT_FOUND != g_pDXGIFactory->EnumAdapterByGpuPreference(adapterId, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()));
            adapterId++)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            auto hr = adapter->GetDesc1(&desc);
            if (FAILED(hr))
            { continue; }

            RefPtr<ID3D12Device> device;
            hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device.GetAddressOf()));
            if (SUCCEEDED(hr))
            {
                if (adapter.Get() == nullptr)
                { g_pDXGIAdapter = adapter.Get(); }

                RefPtr<IDXGIOutput> output;
                hr = adapter->EnumOutputs(0, output.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = device->QueryInterface(IID_PPV_ARGS(&g_pDevice));
                    if (FAILED(hr))
                    {
                        //ELOG("Error : ID3D12Device::QueryInterface() Failed. errcode = 0x%x", hr);
                        return false;
                    }
                    break;
                }
            }
        }
    }

    // デバッグブレークの設定.
    if (desc.EnableDebug)
    {
        auto hr = g_pDevice->QueryInterface(IID_PPV_ARGS(&g_pInfoQueue));
        if (SUCCEEDED(hr))
        {
            if (desc.EnableBreakOnError)
            { g_pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE); }

            if (desc.EnableBreakOnWarning)
            { g_pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE); }

            // 無視するメッセージID.
            D3D12_MESSAGE_ID denyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
            };

            // 無視するメッセージレベル.
            D3D12_MESSAGE_SEVERITY severities[] = {
                D3D12_MESSAGE_SEVERITY_INFO
            };

            D3D12_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs          = _countof(denyIds);
            filter.DenyList.pIDList         = denyIds;
            filter.DenyList.NumSeverities   = _countof(severities);
            filter.DenyList.pSeverityList   = severities;

            g_pInfoQueue->PushStorageFilter(&filter);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      デバイスの終了処理を行います.
//-----------------------------------------------------------------------------
void DeviceTerm()
{
    SafeRelease(g_pDebug);
    SafeRelease(g_pInfoQueue);
    SafeRelease(g_pDevice);
    SafeRelease(g_pDXGIOutput);
    SafeRelease(g_pDXGIAdapter);
    SafeRelease(g_pDXGIFactory);
}

//-----------------------------------------------------------------------------
//      D3D12デバイスを取得します.
//-----------------------------------------------------------------------------
ID3D12Device8* GetD3D12Device()
{ return g_pDevice; }

//-----------------------------------------------------------------------------
//      DXGIファクトリを取得します.
//-----------------------------------------------------------------------------
IDXGIFactory7* GetDXGIFactory()
{ return g_pDXGIFactory; }
