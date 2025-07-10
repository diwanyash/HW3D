#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>

class App {
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
	void ShowRawInputWindow();
private: 
	ImguiManager imgui;
	Window wnd;
	FrameTimer ft;
	int x = 0, y = 0;
	void SpawnSimulationWindows() noexcept;
	float SpeedFactor = 1.2f;
	float color[3] = {0.0f,0.0f,0.07f};
	bool imguivis = true;
	Model nano{ wnd.Gfx(),"E:/chilli game dev/HW3D/model/nano_textured/nanosuit.obj"};
	Camera cam;
	PointLight light;
};