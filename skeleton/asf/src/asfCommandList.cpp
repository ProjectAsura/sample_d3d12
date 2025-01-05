//-----------------------------------------------------------------------------
// File : asfCommandList.cpp
// Desc : Command List Wrapper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <atomic>
#include <asfCommandList.h>
#include <asfLogger.h>


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// CommandList class
///////////////////////////////////////////////////////////////////////////////
class CommandList : public ICommandList
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

    //-------------------------------------------------------------------------
    //! @brief      生成処理を行います.
    //-------------------------------------------------------------------------
    static bool Create(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, ICommandList** ppCmdList)
    {
        if (pDevice == nullptr || ppCmdList == nullptr)
        {
            ELOG("Error : Invalid Argument.");
            return false;
        }

        auto instance = new CommandList();
        if (!instance->Init(pDevice, type))
        {
            instance->Release();
            ELOG("Error : CommandList::Init() Failed.");
            return false;
        }

        *ppCmdList = instance;
        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      参照カウントを増やします.
    //-------------------------------------------------------------------------
    void AddRef() override
    { m_RefCount++; }

    //-------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //-------------------------------------------------------------------------
    void Release() override
    {
        m_RefCount--;
        if (m_RefCount == 0)
        { delete this; }
    }

    //-------------------------------------------------------------------------
    //! @brief      リセット処理を行います.
    //-------------------------------------------------------------------------
    ID3D12GraphicsCommandList6* Reset() override
    {
        // ダブルバッファリング.
        m_Index = (m_Index + 1) & 0x1;

        // コマンドアロケータをリセット.
        m_pAllocator[m_Index]->Reset();

        // コマンドリストをリセット.
        m_pCmdList->Reset(m_pAllocator[m_Index], nullptr);

        return m_pCmdList;
    }

    //-------------------------------------------------------------------------
    //! @brief      D3D12グラフィックスコマンドリストを取得します.
    //-------------------------------------------------------------------------
    ID3D12GraphicsCommandList6* GetD3D12GraphicsCommandList() const override
    { return m_pCmdList; }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::atomic<uint32_t>       m_RefCount      = {};
    ID3D12GraphicsCommandList6* m_pCmdList      = nullptr;
    ID3D12CommandAllocator*     m_pAllocator[2] = {};
    uint8_t                     m_Index         = 0;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    CommandList()
    : m_RefCount(1)
    , m_pCmdList(nullptr)
    , m_Index   (0)
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~CommandList()
    { Term(); }

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type)
    {
        // 引数チェック.
        if (pDevice == nullptr)
        {
            ELOG( "Error : Invalid Argument." );
            return false;
        }

        for(auto i=0; i<2; ++i)
        {
            // コマンドアロケータを生成.
            auto hr = pDevice->CreateCommandAllocator(type, IID_PPV_ARGS(&m_pAllocator[i]));
            if (FAILED(hr))
            {
                ELOG("Error : ID3D12Device::CreateCommandAllocator() Failed. errcode = 0x%x", hr);
                return false;
            }
        }

        // コマンドリストを生成.
        auto hr = pDevice->CreateCommandList(0, type, m_pAllocator[0], nullptr, IID_PPV_ARGS(&m_pCmdList));
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12Device::CreateCommandList() Failed. errcode = 0x%x", hr);
            return false;
        }

        // 生成直後は開きっぱなしの扱いになっているので閉じておく.
        m_pCmdList->Close();

        m_Index = 0;

        // 正常終了.
        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term()
    {
        if (m_pCmdList != nullptr)
        {
            m_pCmdList->Release();
            m_pCmdList = nullptr;
        }

        for(auto i=0; i<2; ++i)
        {
            if (m_pAllocator[i] != nullptr)
            {
                m_pAllocator[i]->Release();
                m_pAllocator[i] = nullptr;
            }
        }

        m_Index = 0;
    }
};

//-----------------------------------------------------------------------------
//      コマンドリストを生成します.
//-----------------------------------------------------------------------------
bool CreateCommandList(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, ICommandList** ppCmdList)
{ return CommandList::Create(pDevice, type, ppCmdList); }

} // namespace asf
