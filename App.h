#pragma once
#include "Window.h"
#include "Box.h"
#include "FrameTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <physx/PxPhysicsAPI.h>
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
	std::vector<std::unique_ptr<Box>> orbiters;
	PxVec3 orbitCenter{ 0.0f, 0.0f, 0.0f };
	std::vector<class Box*> boxes;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
	void SpawnSimulationWindows() noexcept;
	void CreateOrbitingBoxes(Graphics& gfx, PxPhysics* physics, PxScene* scene);
	void UpdateOrbiterForces();
	static constexpr int nDrawables = 20;
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.06f};
	DirectX::XMFLOAT3 suzcolor = {0.0f,1.0f,0.0f};
	bool imguivis = true;
	Camera cam;
	PointLight light;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};