#include "Graphics.h"


GraphicsClass::GraphicsClass(int screenHeight, int screenWidth, HWND hwnd)
	: m_direct3D(new D3DClass(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN))
{
}


bool GraphicsClass::processFrame()
{
	bool result;

	// Render the graphics scene.
	result = render();

	return result;
}


bool GraphicsClass::render()
{
    m_direct3D->render();
	return true;
}