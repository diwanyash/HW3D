#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Model.h"
#include <set>

class App {
public:
	App();
	int Go();
	~App();
private:
	void DoFrame();
private:
	struct
	{
		float roll = 0.0f;
		float pitch =0.0f;
		float yaw =	 0.0f;
		float x =	 0.0f;
		float y =	 0.0f;
		float z =	 0.0f;
	}pos;
private: 
	ImguiManager imgui;
	Window wnd;
	FrameTimer ft;
	void SpawnSimulationWindows() noexcept;
	void SpawnModelWindow();
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.07f};
	bool imguivis = true;
	Model nano{ wnd.Gfx(),"E:/chilli game dev/HW3D/model/nanosuit.obj"};
	Camera cam;
	PointLight light;
};