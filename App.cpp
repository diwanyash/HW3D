#include "App.h"

App::App()
	:
	wnd( 640, 480, "happy window" )
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
}
