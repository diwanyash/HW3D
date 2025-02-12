#include "App.h"

App::App()
	:
	wnd( 800, 600, "happy window" )
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
	wnd.Gfx().ClearBuffer(0.3f, 1.0f, 0.7f);
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}
