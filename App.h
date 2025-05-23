#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"

class App {
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	FrameTimer ft;
	std::vector <std::unique_ptr<class Box>> boxes;
};