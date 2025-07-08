#include "Window.h"
#include "DeadException.h"
#include "sstream"
#include "App.h"

int CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow
	)
{
	try
	{

		/************* Window ****************/
		return App{}.Go();

	}
	catch (const Deadexception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standerd Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr,"No Details Available","Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}






