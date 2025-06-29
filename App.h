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
	std::vector<std::unique_ptr<class Drawable>> drawable;
	static constexpr int nDrawables = 120;
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.0f};
	bool imguivis = true;
	unsigned char e;
	Camera cam;
};