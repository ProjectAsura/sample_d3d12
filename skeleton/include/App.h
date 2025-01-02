//-----------------------------------------------------------------------------
// File : App.h
// Desc : Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <Windows.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////
class App
{
public:
    App(LPCWSTR title, uint32_t width, uint32_t height);
    virtual ~App();
    int Run();

    HINSTANCE GetInstanceHandle () const { return m_hInstance; }
    HWND      GetWindowHandle   () const { return m_hWnd; }
    uint32_t  GetWidth          () const { return m_Width; }
    uint32_t  GetHeight         () const { return m_Height; }

protected:
    virtual bool OnInit     () { return true; }
    virtual void OnTerm     () {}
    virtual void OnRender   () {}
    virtual void OnKeyDown  (uint32_t key) {}
    virtual void OnKeyUp    (uint32_t key) {}
    virtual void OnChar     (uint32_t key) {}
    virtual void OnResize   (uint32_t w, uint32_t h) {}

private:
    HINSTANCE   m_hInstance = NULL;
    HWND        m_hWnd      = NULL;
    uint32_t    m_Width     = 0;
    uint32_t    m_Height    = 0;
    LPCWSTR     m_Title     = NULL;

    bool Init();
    void Term();
    int  MainLoop();

    static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};