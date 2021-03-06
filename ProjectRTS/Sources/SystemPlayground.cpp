#include "SystemPlayground.h"

SystemClass::SystemClass() :
    m_input(new InputClass)
{
    int screenHeight, screenWidth;

    // Initialize the width and height of the screen to zero before sending the variables into the function.
    screenHeight = 0;
    screenWidth = 0;

    // Initialize the windows api.
    initializeWindows(screenHeight, screenWidth);

    // Create the graphics object.  This object will handle rendering all the graphics for this application.
    m_graphics = std::unique_ptr<GraphicsClass> (new GraphicsClass(screenHeight, screenWidth, m_hwnd));
}

void SystemClass::shutdown()
{
	// Shutdown the window.
	shutdownWindows();
}

void SystemClass::run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = processFrame();
			if (!result)
			{
				done = true;
			}
		}

	}
}

bool SystemClass::processFrame()
{
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if (m_input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

    if(m_input->isKeyDown(VK_LEFT) || m_input->isKeyDown('A'))
    {
        m_graphics->getCamera()->moveCamera(DirectX::XMFLOAT3(-0.1f, 0.f, 0.f));
        m_graphics->updateVertexConstantBuffer();
    }
    if(m_input->isKeyDown(VK_RIGHT) || m_input->isKeyDown('D'))
    {
        m_graphics->getCamera()->moveCamera(DirectX::XMFLOAT3(0.1f, 0.f, 0.f));
        m_graphics->updateVertexConstantBuffer();
    }
    if(m_input->isKeyDown(VK_UP) || m_input->isKeyDown('W'))
    {
        m_graphics->getCamera()->moveCamera(DirectX::XMFLOAT3(0.f, 0.f, 0.1f));
        m_graphics->updateVertexConstantBuffer();
    }
    if(m_input->isKeyDown(VK_DOWN) || m_input->isKeyDown('S'))
    {
        m_graphics->getCamera()->moveCamera(DirectX::XMFLOAT3(0.f, 0.f, -0.1f));
        m_graphics->updateVertexConstantBuffer();
    }
    if(m_input->isKeyDown('Q'))
    {
        m_graphics->getCamera()->ratateCamera(1.f);
        m_graphics->updateVertexConstantBuffer();
    }
    if(m_input->isKeyDown('E'))
    {
        m_graphics->getCamera()->ratateCamera(-1.f);
        m_graphics->updateVertexConstantBuffer();
    }

	// Do the frame processing for the graphics object.
	result = m_graphics->render();

	return result;
}

LRESULT CALLBACK SystemClass::messageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
		// If a key is pressed send it to the input object so it can record that state.
		m_input->keyDown((unsigned int)wparam);
        break;

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
		// If a key is released then send it to the input object so it can unset the state for that key.
		m_input->keyUp((unsigned int)wparam);
        break;

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

    return 0;
}

void SystemClass::initializeWindows(int& screenHeight, int& screenWidth)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Project RTS";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	screenWidth = GetSystemMetrics(SM_CXSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_graphics->FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);
}

void SystemClass::shutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (m_graphics->FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// Check if the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
        break;

	// Check if the window is being closed.
	case WM_CLOSE:
		PostQuitMessage(0);
        break;

	// All other messages pass to the message handler in the system class.
	default:
		return ApplicationHandle->messageHandler(hwnd, umessage, wparam, lparam);
	}

    return 0;
}