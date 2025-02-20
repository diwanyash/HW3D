#include "App.h"
#define gfx wnd.Gfx()

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
		gfx.ClearBuffer();
		Logic();
		DrawFrame();
		gfx.EndFrame();
	}
}

void App::Logic()
{
}
void App::DrawFrame()
{
}
