#include "Graphics.h"

#include <d3d12.h>
#include <array>

GraphicsClass::GraphicsClass(int screenHeight, int screenWidth, HWND hwnd)
	: m_pDirect3D(new D3DClass(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN))
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
    m_pDirect3D->render();
	return true;
}