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
private: 
	ImguiManager imgui;
	Window wnd;
	FrameTimer ft;
	void SpawnSimulationWindows() noexcept;
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.07f};
	bool imguivis = true;
	Model nano{ wnd.Gfx(),"E:/chilli game dev/HW3D/model/nano.gltf"};
	Camera cam;
	PointLight light;
};