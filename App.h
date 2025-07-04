#pragma once
#include "Window.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
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
	std::vector<std::unique_ptr<class Drawable>> drawable;
	std::vector<class Box*> boxes;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
	void SpawnSimulationWindows() noexcept;
	static constexpr int nDrawables = 120;
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.06f};
	DirectX::XMFLOAT3 suzcolor = {0.0f,1.0f,0.0f};
	bool imguivis = true;
	Camera cam;
	PointLight light;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};