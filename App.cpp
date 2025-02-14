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
	if (wnd.kbd.KeyIsPressed('Q'))
	{
		input = 0;
	}
	else if (wnd.kbd.KeyIsPressed('W'))
	{
		input = 1;
	}
	else if (wnd.kbd.KeyIsPressed('A'))
	{
		input = 2;
	}
	else if (wnd.kbd.KeyIsPressed('S'))
	{
		input = 3;
	}

	wnd.Gfx().DrawTestTriangle(input, ft.Peek(),wnd.mouse.GetPosX() / 400.0f - 1.0f, wnd.mouse.GetPosY() / 300.0f - 1.0f);
	wnd.Gfx().EndFrame();
}
