#include "Graphics.h"


GraphicsClass::GraphicsClass(int screenHeight, int screenWidth, HWND hwnd)
	: m_direct3D(new D3DClass(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN))
{
}


bool GraphicsClass::Frame()
{
	bool result;

	// Render the graphics scene.
	result = Render();

	return result;
}


bool GraphicsClass::Render()
{
    m_direct3D->Render();
	return true;
}