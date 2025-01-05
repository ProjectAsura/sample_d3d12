//-----------------------------------------------------------------------------
// File : asfLogger.h
// Desc : Logger Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdio>
#include <cstdarg>
#include <atomic>
#include <Windows.h>
#include <asfLogger.h>


namespace asf {

namespace {

//-----------------------------------------------------------------------------
// Global Variables.
//-----------------------------------------------------------------------------
static std::atomic<bool>            g_Init = {};
static CONSOLE_SCREEN_BUFFER_INFO   g_Info = {};

//-----------------------------------------------------------------------------
//      カラーを設定します.
//-----------------------------------------------------------------------------
void SetColor(LOG_LEVEL level)
{
    auto handle    = GetStdHandle(STD_OUTPUT_HANDLE);
    auto attribute = g_Info.wAttributes;

    switch(level)
    {
    case LOG_VERBOSE:
        attribute = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;

    case LOG_INFO:
        attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;

    case LOG_DEBUG:
        attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;

    case LOG_WARNING:
        attribute = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;

    case LOG_ERROR:
        attribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute(handle, attribute);
}

//-----------------------------------------------------------------------------
//      デフォルトカラーを設定します.
//-----------------------------------------------------------------------------
void SetDefaultColor()
{
    auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, g_Info.wAttributes);
}

} // namespace

///////////////////////////////////////////////////////////////////////////////
// DefaultLogger class
///////////////////////////////////////////////////////////////////////////////
class DefaultLogger : public ILogger
{
public:
    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    DefaultLogger() = default;

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~DefaultLogger() = default;

    //-------------------------------------------------------------------------
    //! @brief      ログを出力します.
    //-------------------------------------------------------------------------
    void WriteA(LOG_LEVEL level, const char* format, ...) override
    {
        if (!g_Init)
        {
            const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(handle, &g_Info);
            g_Init = true;
        }

        char msg[1024] = "\0";
        va_list arg;

        va_start(arg, format);
        vsprintf_s(msg, format, arg);
        va_end(arg);

        SetColor(level);
        fprintf((level == LOG_ERROR ? stderr : stdout), "%s", msg);
        SetDefaultColor();

        OutputDebugStringA(msg);
    }

    //-------------------------------------------------------------------------
    //! @brief      ログを出力します.
    //-------------------------------------------------------------------------
    void WriteW(LOG_LEVEL level, const wchar_t* format, ... ) override
    {
        if (!g_Init)
        {
            const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(handle, &g_Info);
            g_Init = true;
        }

        wchar_t msg[1024] = L"\0";
        va_list arg;

        va_start(arg, format);
        vswprintf_s(msg, format, arg);
        va_end(arg);

        SetColor(level);
        fwprintf_s((level == LOG_ERROR ? stderr : stdout), L"%s", msg);
        SetDefaultColor();

        OutputDebugStringW(msg);
    }
} g_DefaultLogger;

//-----------------------------------------------------------------------------
//      デフォルトロガーを設定します.
//-----------------------------------------------------------------------------
ILogger* GetDefaultLogger()
{ return &g_DefaultLogger; }

} // namespace asf
