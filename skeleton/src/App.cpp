//-----------------------------------------------------------------------------
// File : App.cpp
// Desc : Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
App::App(LPCWSTR title, uint32_t width, uint32_t height)
: m_hInstance   (NULL)
, m_hWnd        (NULL)
, m_Width       (width)
, m_Height      (height)
, m_Title       (title)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
App::~App()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      アプリケーションを実行します.
//-----------------------------------------------------------------------------
int App::Run()
{
    int ret = -1;
    if (Init())
    { ret = MainLoop(); }

    Term();
    return ret;
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool App::Init()
{
    m_hInstance = GetModuleHandleW(nullptr);
    if (!m_hInstance)
    { return false; }

    WNDCLASSEXW wc = {};
    wc.cbSize           = sizeof(WNDCLASSEXA);
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = WndProc;
    wc.hInstance        = m_hInstance;
    wc.hCursor          = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName    = L"SampleClass";
    RegisterClassExW(&wc);

    RECT rc = { 0, 0, LONG(m_Width), LONG(m_Height) };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindowW(
        wc.lpszClassName,
        m_Title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        m_hInstance,
        this);

    if (!m_hWnd)
    { return false; }

    if (!OnInit())
    { return false; }

    UpdateWindow(m_hWnd);
    ShowWindow(m_hWnd, SW_SHOWNORMAL);

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void App::Term()
{
    OnTerm();

    UnregisterClassW(L"SampleClass", m_hInstance);

    m_hInstance = NULL;
    m_hWnd = NULL;
}

//-----------------------------------------------------------------------------
//      メインループ処理です.
//-----------------------------------------------------------------------------
int App::MainLoop()
{
    MSG msg = {};
    while(msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return int(msg.wParam);
}

//-----------------------------------------------------------------------------
//      ウィンドウプロシージャです.
//-----------------------------------------------------------------------------
LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    auto instance = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch(msg)
    {
    case WM_CREATE:
        {
            LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lp);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
        }
        return 0;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (instance)
        { instance->OnKeyDown(uint32_t(wp)); }
        return 0;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (instance)
        { instance->OnKeyUp(uint32_t(wp)); }
        return 0;

    case WM_SIZE:
        if (instance)
        {
            auto w = uint32_t(LOWORD(lp));
            auto h = uint32_t(HIWORD(lp));
            instance->m_Width  = w;
            instance->m_Height = h;
            instance->OnResize(w, h);
        }
        return 0;

    case WM_CHAR:
        if (instance)
        { instance->OnChar(uint32_t(wp)); }
        return 0;

    case WM_PAINT:
        if (instance)
        { instance->OnRender(); }
        return 0;

    case WM_DESTROY:
        { PostQuitMessage(0); }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wp, lp);
}