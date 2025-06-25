#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"

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
	std::vector <std::unique_ptr<class Melon>> melons;
	float SpeedFactor = 0.7f;
	float color[3] = {0.0f,0.0f,0.0f};
	Camera cam;
};