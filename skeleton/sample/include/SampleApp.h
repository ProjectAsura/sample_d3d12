//-----------------------------------------------------------------------------
// File : SampleApp.h
// Desc : Sample Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asfApp.h>


///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////
class SampleApp : public asf::App
{
public:
    SampleApp();
    ~SampleApp();


private:

    bool OnInit     () override;
    void OnTerm     () override;
    void OnRender   () override;
    void OnKeyDown  (uint32_t key) override;
    void OnKeyUp    (uint32_t key) override;
    void OnChar     (uint32_t key) override;
    void OnResize   (uint32_t w, uint32_t h) override;
    void OnMouse    (int x, int y, int wheel, bool left, bool middle, bool right, bool side1, bool side2) override;
};