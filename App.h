#pragma once
#include "Window.h"


class App {
public:
	App();
	int Go();
private:
	void DrawFrame();
	void Logic();
private:
	Window wnd = Window(Graphics::WIDTH, Graphics::HEIGHT, "AAA GAME!!!");
	Graphics& gfx = wnd.Gfx();
	/*/////////////////////////
	USER DATA BEGIN
	*/////////////////////////
private:
	int posx = 0;
	int posy = 0;
	/*/////////////////////////
	USER DATA END
	*/////////////////////////
};