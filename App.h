#pragma once
#include "Window.h"
#include "FrameTimer.h"

class App {
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	Window wnd;
	FrameTimer ft;
	std::vector <std::unique_ptr<class Box>> boxes;
};