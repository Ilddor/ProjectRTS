#include "SystemPlayground.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass system;
	bool result;


	// Create the system object.
    system.Run();

	// Shutdown and release the system object.
	system.Shutdown();

	return 0;
}