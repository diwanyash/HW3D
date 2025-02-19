#pragma once
#include "Window.h"


class App {
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd = Window(Graphics::WIDTH, Graphics::HEIGHT, "AAA GAME!!!");
	Graphics& gfx = wnd.Gfx();
};