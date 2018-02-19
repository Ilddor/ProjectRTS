#include "SystemPlayground.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
	SystemClass system;

	// Create the system object.
    system.run();

	// Shutdown and release the system object.
	system.shutdown();

	return 0;
}