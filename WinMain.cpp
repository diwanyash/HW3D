#include "Window.h"
#include "DeadException.h"
#include "sstream"

int CALLBACK WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow
	)
{
	try
	{
		Window wnd = { 640,480,"happy window" };
		// Message Process
		MSG msg;
		BOOL gResult;
		int wheel = 0;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				switch (e.GetType())
				{
					case Mouse::Event::Type::WheelUp:
					{
						wheel++;
						std::ostringstream oss;
						oss << wheel;
						wnd.SetTitle(oss.str());
					}
					break;
					case Mouse::Event::Type::WheelDown:
					{
						wheel--;
						std::ostringstream oss;
						oss << wheel;
						wnd.SetTitle(oss.str());
					}
					break;
				}
			}
		}
		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
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
}







