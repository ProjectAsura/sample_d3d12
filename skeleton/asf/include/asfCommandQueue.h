﻿//----------------------------------------------------------------------------
// File : asfCommandQueue.h
// Desc : Command Queue Wrapper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <d3d12.h>


namespace asf {

//-----------------------------------------------------------------------------
// Type Definitions.
//-----------------------------------------------------------------------------
using WaitPoint = uint64_t;


///////////////////////////////////////////////////////////////////////////////
// ICommandQueue interface
///////////////////////////////////////////////////////////////////////////////
struct ICommandQueue
{
    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~ICommandQueue() {}

    //-------------------------------------------------------------------------
    //! @brief      参照カウンタを増やします.
    //-------------------------------------------------------------------------
    virtual void AddRef() = 0;

    //-------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //-------------------------------------------------------------------------
    virtual void Release() = 0;

    //-------------------------------------------------------------------------
    //! @brief      コマンドを実行します.
    //! 
    //! @param[in]      count       実行するコマンドリストの数.
    //! @param[in]      ppLists     コマンドリストの配列です.
    //-------------------------------------------------------------------------
    virtual void Execute(uint32_t count, ID3D12CommandList** ppLists) = 0;

    //-------------------------------------------------------------------------
    //! @brief      フェンスの値を更新します.
    //! 
    //! @return     GPU待機点を返却します.
    //-------------------------------------------------------------------------
    virtual WaitPoint Signal() = 0;

    //-------------------------------------------------------------------------
    //! @brief      GPUでの待機点を設定します.
    //! 
    //! @param[in]      value       GPU待機点.
    //! @retval true    処理に成功.
    //! @retval false   処理に失敗.
    //-------------------------------------------------------------------------
    virtual bool Wait(const WaitPoint& value) = 0;

    //-------------------------------------------------------------------------
    //! @brief      CPU上でコマンドの完了を待機します.
    //! 
    //! @param[in]      value       GPU待機点
    //! @param[in]      msec        待機時間をミリ秒単位で設定します.
    //-------------------------------------------------------------------------
    virtual void Sync(const WaitPoint& value, uint32_t msec) = 0;

    //-------------------------------------------------------------------------
    //! @brief      タイムスタンプ周波数を取得します.
    //! 
    //! @return     タイムスタンプ周波数を返却します.
    //-------------------------------------------------------------------------
    virtual uint64_t GetTimeStampFrequency() const = 0;

    //-------------------------------------------------------------------------
    //! @brief      D3D12コマンドキューを取得します.
    //! 
    //! @return     D3D12コマンドキューを返却します.
    //-------------------------------------------------------------------------
    virtual ID3D12CommandQueue* GetD3D12CommandQueue() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// IFence interface
///////////////////////////////////////////////////////////////////////////////
struct IFence
{
    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~IFence() {}

    //-------------------------------------------------------------------------
    //! @brief      参照カウンタを増やします.
    //-------------------------------------------------------------------------
    virtual void AddRef() = 0;

    //-------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //-------------------------------------------------------------------------
    virtual void Release() = 0;

    //-------------------------------------------------------------------------
    //! @brief      フェンスが指定された値に達するまで待機します.
    //! 
    //! @param[in]      fenceValue      待機カウンタ.
    //! @param[in]      msec            タイムアウト時間(ミリ秒).
    //-------------------------------------------------------------------------
    virtual void Wait(uint64_t fenceValue, uint32_t msec) = 0;

    //-------------------------------------------------------------------------
    //! @brief      D3D12フェンスを取得します.
    //! 
    //! @return     D3D12フェンスを返却します.
    //-------------------------------------------------------------------------
    virtual ID3D12Fence* GetD3D12Fence() const = 0;
};


//-----------------------------------------------------------------------------
//! @brief      コマンドキューを生成します.
//! 
//! @param[in]      pDevice     デバイスです.
//! @param[in]      type        コマンドリストタイプです.
//! @param[out]     ppQueue     コマンドリストキューの格納先です.
//! @retval true    生成に成功.
//! @retval false   生成に失敗.
//-----------------------------------------------------------------------------
bool CreateCommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, ICommandQueue** ppQueue);

//-----------------------------------------------------------------------------
//! @brief      フェンスを生成します.
//! 
//! @param[in]      pDevice     デバイスです.
//! @param[out]     ppFence     フェンスの格納先です.
//! @retval true    生成に成功.
//! @retval false   生成に失敗.
//-----------------------------------------------------------------------------
bool CreateFence(ID3D12Device* pDevice, IFence** ppFence);

} // namespace asf
