#include "App.h"

App::App()
{}

int App::Go()
{
	while (true)
	{
		if ( const auto ecode = Window::ProcessMessage() )
		{
			return *ecode;
		}
		DoFrame();

	}
}

void App::DoFrame()
{
	wnd.Gfx().ClearBuffer();
	//wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().SetPixel(100, 100, Colors::Red);
	wnd.Gfx().SetPixel(101, 100, Colors::Blue);
	wnd.Gfx().SetPixel(102, 100, Colors::Red);
	wnd.Gfx().EndFrame();
}
