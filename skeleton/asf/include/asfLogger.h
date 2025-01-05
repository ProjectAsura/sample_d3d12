//-----------------------------------------------------------------------------
// File : asfLogger.h
// Desc : Logger.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once


namespace asf {

///////////////////////////////////////////////////////////////////////////////
// LOG_LEVEL enum
///////////////////////////////////////////////////////////////////////////////
enum LOG_LEVEL
{
    LOG_VERBOSE = 0,          //!< VERBOSEレベル (白).
    LOG_INFO,                 //!< INFOレベル    (緑).
    LOG_DEBUG,                //!< DEBUGレベル   (青).
    LOG_WARNING,              //!< WARNINGレベル (黄).
    LOG_ERROR,                //!< ERRORレベル   (赤).
};


///////////////////////////////////////////////////////////////////////////////
// ILogger interface
///////////////////////////////////////////////////////////////////////////////
struct ILogger
{
    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~ILogger() {}

    //-------------------------------------------------------------------------
    //! @brief      ログを出力します.
    //!
    //! @param[in]      level       ログレベルです.
    //! @param[in]      format      フォーマットです.
    //-------------------------------------------------------------------------
    virtual void WriteA(LOG_LEVEL level, const char* format, ... ) = 0;

    //-------------------------------------------------------------------------
    //! @brief      ログを出力します.
    //!
    //! @param[in]      level       ログレベルです.
    //! @param[in]      format      フォーマットです.
    //-------------------------------------------------------------------------
    virtual void WriteW(LOG_LEVEL level, const wchar_t* format, ... ) = 0;
};

//-----------------------------------------------------------------------------
//! @brief      デフォルトロガーを取得します.
//-----------------------------------------------------------------------------
ILogger* GetDefaultLogger();

} // namespace asf


//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#ifndef __ASF_WIDE
#define __ASF_WIDE( _string )      L ## _string
#endif//__ASF_WIDE

#ifndef ASF_WIDE
#define ASF_WIDE( _string )        __ASF_WIDE( _string )
#endif//ASF_WIDE

#ifndef DLOGA
  #if defined(DEBUG) || defined(_DEBUG)
    #define DLOGA( fmt, ... )      asf::GetDefaultLogger()->WriteA( asf::LOG_DEBUG, "[File: %s, Line: %d] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__ )
  #else
    #define DLOGA( fmt, ... )      ((void)0)
  #endif//defined(DEBUG) || defined(_DEBUG)
#endif//DLOGA

#ifndef DLOGW
  #if defined(DEBUG) || defined(_DEBUG)
    #define DLOGW( fmt, ... )      asf::GetDefaultLogger()->WriteW( asf::LOG_DEBUG, ASDX_WIDE("[File: %s, Line: %d] ") ASF_WIDE(fmt) ASF_WIDE("\n"), ASF_WIDE(__FILE__), __LINE__, ##__VA_ARGS__ )
  #else
    #define DLOGW( fmt, ... )      ((void)0)
  #endif//defined(DEBUG) || defined(_DEBUG)
#endif//DLOGW

#ifndef VLOGA
#define VLOGA( fmt, ... )   asf::GetDefaultLogger()->WriteA( asf::LOG_VERBOSE, fmt "\n", ##__VA_ARGS__ )
#endif//VLOGA

#ifndef VLOGW
#define VLOGW( fmt, ... )   asf::GetDefaultLogger()->WriteW( asf::LOG_VERBOSE, ASF_WIDE(fmt) ASF_WIDE("\n"), ##__VA_ARGS__ )
#endif//VLOGW

#ifndef ILOGA
#define ILOGA( fmt, ... )   asf::GetDefaultLogger()->WriteA( asf::LOG_INFO, fmt "\n", ##__VA_ARGS__ )
#endif//ILOGA

#ifndef ILOGW
#define ILOGW( fmt, ... )   asf::GetDefaultLogger()->WriteW( asf::LOG_INFO, ASF_WIDE(fmt) ASF_WIDE("\n"), ##__VA_ARGS__ );
#endif//ILOGW

#ifndef WLOGA
#define WLOGA( fmt, ... )   asf::GetDefaultLogger()->WriteA( asf::LOG_WARNING, fmt "\n", ##__VA_ARGS__ )
#endif//WLOGA

#ifndef WLOGW
#define WLOGW( fmt, ... )   asf::GetDefaultLogger()->WriteW( asf::LOG_WARNING, ASF_WIDE(fmt) ASF_WIDE("\n"), ##__VA_ARGS__ )
#endif//WLOGW

#ifndef ELOGA
#define ELOGA( fmt, ... )   asf::GetDefaultLogger()->WriteA( asf::LOG_ERROR, "[File: %s, Line: %d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOGA

#ifndef ELOGW
#define ELOGW( fmt, ... )   asf::GetDefaultLogger()->WriteW( asf::LOG_ERROR, ASF_WIDE("[File: %s, Line: %d] ") ASF_WIDE(fmt) ASF_WIDE("\n"), ASF_WIDE(__FILE__), __LINE__, ##__VA_ARGS__ )
#endif//ELOGW

#if defined(UNICODE) || defined(_UNICODE)
    #define VLOG        VLOGW
    #define DLOG        DLOGW
    #define ILOG        ILOGW
    #define WLOG        WLOGW
    #define ELOG        ELOGW
#else
    #define VLOG        VLOGA 
    #define DLOG        DLOGA
    #define ILOG        ILOGA
    #define WLOG        WLOGA 
    #define ELOG        ELOGA
#endif