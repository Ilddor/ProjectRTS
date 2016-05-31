#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <memory>

#include "Input.h"
#include "Graphics.h"

class SystemClass
{
public:
	SystemClass();

	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool RenderFrame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

    std::unique_ptr<InputClass> m_input;
    std::unique_ptr<GraphicsClass> m_graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;