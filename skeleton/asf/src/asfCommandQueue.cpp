//-----------------------------------------------------------------------------
// File : asfCommandQueue.cpp
// Desc : Command Queue Wrapper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <atomic>
#include <asfCommandQueue.h>
#include <asfLogger.h>


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// Fence class
///////////////////////////////////////////////////////////////////////////////
class Fence : public IFence
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
    static bool Create(ID3D12Device* pDevice, IFence** ppFence)
    {
        if (pDevice == nullptr || ppFence == nullptr)
        {
            ELOG("Error : Invalid Argument.");
            return false;
        }

        auto instance = new Fence();
        if (!instance->Init(pDevice))
        {
            instance->Release();
            ELOG("Error : Fence::Init() Failed.");
            return false;
        }

        *ppFence = instance;
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
    //! @brief      フェンスが指定された値に達するまで待機します.
    //-------------------------------------------------------------------------
    void Wait(uint64_t fenceValue, uint32_t msec) override
    {
        if (m_pFence->GetCompletedValue() < fenceValue)
        {
            auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_Handle);
            if (FAILED(hr))
            {
                ELOG("Error : ID3D12Fence::SetEventOnCompletation() Failed. errcode = 0x%x", hr);
                return;
            }

            WaitForSingleObject(m_Handle, msec);
        }
    }

    //-------------------------------------------------------------------------
    //! @brief      D3D12フェンスを取得します.
    //-------------------------------------------------------------------------
    ID3D12Fence* GetD3D12Fence() const override
    { return m_pFence; }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::atomic<uint32_t>   m_RefCount  = {};
    ID3D12Fence*            m_pFence    = nullptr;
    HANDLE                  m_Handle    = nullptr;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    Fence()
    : m_RefCount(1)
    , m_pFence  (nullptr)
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~Fence()
    { Term(); }

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(ID3D12Device* pDevice)
    {
        // 引数チェック.
        if (pDevice == nullptr)
        {
            ELOG("Error : Invalid Error.");
            return false;
        }

        // イベントを生成.
        m_Handle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        if (m_Handle == nullptr)
        {
            ELOG("Error : CreateEventW() Failed.");
            return false;
        }

        // フェンスを生成.
        auto hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12Device::CreateFence() Failed. errcode = 0x%x", hr);
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term()
    {
        if (m_Handle != nullptr)
        {
            CloseHandle(m_Handle);
            m_Handle = nullptr;
        }

        if (m_pFence != nullptr)
        {
            m_pFence->Release();
            m_pFence = nullptr;
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// CommandQueue class
///////////////////////////////////////////////////////////////////////////////
class CommandQueue : public ICommandQueue
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
    static bool Create(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, ICommandQueue** ppQueue)
    {
        if (pDevice == nullptr || ppQueue == nullptr)
        {
            ELOG("Error : Invalid Argument.");
            return false;
        }

        auto instance = new CommandQueue();
        if (!instance->Init(pDevice, type))
        {
            instance->Release();
            ELOG("Error : CommandQueue::Init() Failed.");
            return false;
        }

        *ppQueue = instance;
        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      参照カウンタを増やします.
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
    //! @brief      コマンドリストを実行します.
    //-------------------------------------------------------------------------
    void Execute(uint32_t count, ID3D12CommandList** ppLists) override
    {
        if(count == 0 || ppLists == nullptr)
        { return; }

        m_pQueue->ExecuteCommandLists(count, ppLists);
        m_IsExecuted = true;
    }

    //-------------------------------------------------------------------------
    //! @brief      フェンス値を更新します.
    //-------------------------------------------------------------------------
    WaitPoint Signal() override
    {
        WaitPoint result = {};

        const auto fence = m_FenceValue;
        auto hr = m_pQueue->Signal(m_pFence->GetD3D12Fence(), fence);
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12CommandQueue::Signal() Failed. errcode = 0x%x", hr);
            return result;
        }
        m_FenceValue++;
        result = fence;

        return result;
    }

    //-------------------------------------------------------------------------
    //! @brief      GPUでの待機点を設定します.
    //-------------------------------------------------------------------------
    bool Wait(const WaitPoint& value) override
    {
        auto hr = m_pQueue->Wait(m_pFence->GetD3D12Fence(), value);
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12CommandQueue::Wait() Failed. errcode = 0x%x", hr);
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      CPU上でコマンドの完了を待機します.
    //-------------------------------------------------------------------------
    void Sync(const WaitPoint& value, uint32_t msec) override
    {
        if (!m_IsExecuted)
        { return; }

        m_pFence->Wait(value, msec);
    }

    //-------------------------------------------------------------------------
    //! @brief      タイムスタンプ周波数を取得します.
    //-------------------------------------------------------------------------
    uint64_t GetTimeStampFrequency() const override
    {
        uint64_t result = 0;
        auto hr = m_pQueue->GetTimestampFrequency(&result);
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12CommandQueue::GetTimeStampFrequency() Failed. errcode = 0x%x", hr);
        }
        return result;
    }

    //-------------------------------------------------------------------------
    //! @brief      D3D12コマンドキューを取得します.
    //-------------------------------------------------------------------------
    ID3D12CommandQueue* GetD3D12CommandQueue() const override
    { return m_pQueue; }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    std::atomic<uint32_t>   m_RefCount   = {};
    ID3D12CommandQueue*     m_pQueue     = nullptr;
    IFence*                 m_pFence     = nullptr;
    std::atomic<bool>       m_IsExecuted = {};
    uint64_t                m_FenceValue = 0;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    CommandQueue()
    : m_RefCount    (1)
    , m_pQueue      (nullptr)
    , m_pFence      (nullptr)
    , m_IsExecuted  (false)
    , m_FenceValue  (0)
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~CommandQueue()
    { Term(); }

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type)
    {
        if (pDevice == nullptr)
        {
            ELOG( "Error : Invalid Arugment." );
            return false;
        }

        if (!Fence::Create(pDevice, &m_pFence))
        { return false; }

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type       = type;
        desc.Priority   = 0;
        desc.Flags      = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask   = 1;

        auto hr = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
        if (FAILED(hr))
        {
            ELOG("Error : ID3D12Device::CreateCommandQueue() Failed. errcodes = 0x%x", hr);
            return false;
        }

        m_IsExecuted = false;
        m_FenceValue = 1;

        return true;
    }

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term()
    {
        if (m_pFence != nullptr)
        {
            m_pFence->Release();
            m_pFence = nullptr;
        }

        if (m_pQueue != nullptr)
        {
            m_pQueue->Release();
            m_pQueue = nullptr;
        }

        m_IsExecuted = false;
        m_FenceValue = 0;
    }
};

//-----------------------------------------------------------------------------
//      コマンドキューを生成します.
//-----------------------------------------------------------------------------
bool CreateCommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, ICommandQueue** ppQueue)
{ return CommandQueue::Create(pDevice, type, ppQueue); }

//-----------------------------------------------------------------------------
//      フェンスを生成します.
//-----------------------------------------------------------------------------
bool CreateFence(ID3D12Device* pDevice, IFence** ppFence)
{ return Fence::Create(pDevice, ppFence); }

} // namespace asf
