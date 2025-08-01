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
	static constexpr int nDrawables = 0;
	float SpeedFactor = 0.4f;
	float color[3] = {0.0f,0.0f,0.06f};
	DirectX::XMFLOAT3 suzcolor = {0.0f,1.0f,0.0f};
	bool imguivis = true;
	Camera cam;
	PointLight light;
	std::uniform_real_distribution<float> distancernd{ 0.0f,10.0f};
	std::mt19937 rngg{ std::random_device{}() };
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
	// orbit
	void DrawOrbitDebug(Graphics& gfx);
	bool showOrbitDebug =  false;
	int numOrbitingBoxes = 20;
	float orbitSpeed = 10.0f;
	float forceScale = 100.0f;
	float initialRadius = 10.0f;
	float boxSize = 1.0f;
	bool orbitUseGravity = false;
	bool applyForceContinuously = true;
	bool showOrbitSettings = true;
	DirectX::XMFLOAT3 orbitBoxColor = { 1.0f, 0.5f, 0.2f };
};