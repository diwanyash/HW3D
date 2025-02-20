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
	if (wnd.kbd.KeyIsPressed(VK_LEFT))
	{
		posx--;
	}
	if (wnd.kbd.KeyIsPressed(VK_RIGHT))
	{
		posx++;
	}
	if (wnd.kbd.KeyIsPressed(VK_UP))
	{
		posy--;
	}
	if (wnd.kbd.KeyIsPressed(VK_DOWN))
	{
		posy++;
	}
}
void App::DrawFrame()
{
	for (int i = 10; i < 110;i++)
	{
		for (int j = 10;j < 110;j++)
		{
			gfx.SetPixel(posx + j,posy + i, Colors::Pink);
		}
	}
}
