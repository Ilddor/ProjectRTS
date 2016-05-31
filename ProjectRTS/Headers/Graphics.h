#pragma once

#include <windows.h>
#include <memory>

#include "D3D.h"

class GraphicsClass
{
public:
	GraphicsClass(int screenHeight, int screenWidth, HWND hwnd);
	bool Frame();

    static const bool FULL_SCREEN = false;
    static const bool VSYNC_ENABLED = true;
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;

private:
	bool Render();

    std::unique_ptr<D3DClass> m_direct3D;
};