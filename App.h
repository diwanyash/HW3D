#pragma once
#include "Window.h"
#include "FrameTimer.h"

class App {
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	int input = 0;
	FrameTimer ft;
};