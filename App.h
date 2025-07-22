#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>
#include "PlaneTesting.h"
#include "TestCube.h"

// MERGED BOTH BRANCHES INTO MASTER

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
	float color[3] = {0.07f,0.0f,0.12f};
	bool imguivis = true;
	Model wall{ wnd.Gfx(),"E:/chilli game dev/HW3D/model/brick_wall/brick_wall.obj"};
	Camera cam;
	PointLight light;
};