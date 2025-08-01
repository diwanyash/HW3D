#include "App.h"
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include <DirectXMath.h>
#include "Drawable.h"
#include <memory>
#include "imgui/imgui.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "SkinnedCube.h"
#include "Cylinder.h"
#include "AssImpTest.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "PhysXWrapper.h"
#include <iostream>

namespace dx = DirectX;
GDIPlusManager gdi;

App::App()
	:
	wnd(800, 600, "happy window"),
	light( wnd.Gfx(), 0.5f)
{
	auto& gfx = wnd.Gfx();
	auto* physics = PhysXWrapper::Get()->GetgPhysics();
	auto* scene = PhysXWrapper::Get()->GetgScene();

	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const dx::XMFLOAT3 mat = { cdist(rng),  cdist(rng),  cdist(rng), };
			const dx::XMFLOAT3 mat2 = { 0.0f, 1.0f, 0.0f };

			switch ( typedist( rng ) )
			{
			case 0:
				return std::make_unique<Pyramid>(
				gfx, rng, adist, ddist, odist, rdist, tdist);
				break;
			case 1:
				return std::make_unique<Cylinder>(
				gfx, rng, adist, ddist, odist, rdist,tdist);
				break;
			case 2:
				return std::make_unique<Box>(
				gfx, rng, adist, ddist, odist, rdist, bdist, mat);
				break;
			case 3:
				return std::make_unique<SkinnedCube>(
				gfx, rng, adist, ddist, odist, rdist );
				break;
			case 4:
				return std::make_unique<AssImpTest>(
					gfx, rng, adist, ddist, odist, rdist, mat, 2.0f);
				break;
			//case 4:
			//	return std::make_unique<Sheet>(
			//	gfx, rng, adist, ddist, odist, rdist );
			//	break;
			default:
				assert( false && "Bad Drawable type in factory" );
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f, 3.1415f * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, 3.1415f * 2.0f };
		std::uniform_real_distribution<float> odist{ 0.0f, 3.1415f * 0.3f };
		std::uniform_real_distribution<float> rdist{ 10.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 1.0f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> typedist{ 2, 2 };
		std::uniform_int_distribution<int> tdist{ 3, 30 };
	};

	Factory factory( gfx );
	drawable.reserve( nDrawables );
	std::generate_n( std::back_inserter(drawable), nDrawables, factory );

	for ( auto& pd : drawable)
	{
		if ( auto pb = dynamic_cast<Box*>(pd.get()) ) 
		{
			boxes.push_back(pb);
		}
	}

	CreateOrbitingBoxes(gfx, physics, scene);

	gfx.SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 1000.0f));
}

int App::Go()
{
	while (true)
	{
		if ( const auto ecode = Window::ProcessMessage() )
		{

			return *ecode;
		}
		DoFrame();

	}
}

App::~App()
{
}

void App::DoFrame()
{
	float dt = ft.Mark() * SpeedFactor;

	wnd.Gfx().BeginFrame(color[0], color[1], color[2]);
	PhysXWrapper::Get()->StepSimulation(dt);
	PhysXWrapper::Get()->GetgScene()->simulate(dt);      // e.g. 1.0f / 60.0f
	PhysXWrapper::Get()->GetgScene()->fetchResults(true);
	UpdateOrbiterForces();
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind( wnd.Gfx(), cam.GetMatrix() );

	while( !wnd.kbd.KeyIsEmpty() )
	{
		if (wnd.kbd.KeyIsPressed('I'))
		{
			if (imguivis)
			{
				imguivis = false;
				wnd.Gfx().DisableImgui();
			}
			else
			{
				imguivis = true;
				wnd.Gfx().EnableImgui();
			}
		}
		wnd.kbd.Flush();
	}



	for (auto& d : drawable)
	{
		d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}	
	for ( auto& o : orbiters)
	{
		o->Update(dt);
		o->Draw(wnd.Gfx());
	}
	light.Draw( wnd.Gfx() );

	// ImGui Windows
	SpawnSimulationWindows();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();

	// orbits vars
	if (showOrbitDebug)
		DrawOrbitDebug(wnd.Gfx());
	auto* physics = PhysXWrapper::Get()->GetgPhysics();
	auto* scene = PhysXWrapper::Get()->GetgScene();
	// imgui orbit
	if (showOrbitSettings && ImGui::Begin("Orbit Settings", &showOrbitSettings))
	{
		ImGui::Text("Orbiting Parameters");

		// Orbit Dynamics
		ImGui::SliderFloat("Orbit Speed", &orbitSpeed, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("Force Scale", &forceScale, 0.0f, 1000.0f, "%.1f");
		ImGui::SliderFloat("Orbit Radius", &initialRadius, 1.0f, 50.0f, "%.1f");
		ImGui::SliderInt("Box Count", &numOrbitingBoxes, 1, 100);
		ImGui::Checkbox("Enable Gravity", &orbitUseGravity);
		ImGui::Checkbox("Apply Force Every Frame", &applyForceContinuously);
		ImGui::Checkbox("Show Orbit Debug", &showOrbitDebug);

		ImGui::Separator();

		// Box Size and Material
		ImGui::Text("Box Parameters");
		ImGui::SliderFloat("Box Size", &boxSize, 0.1f, 5.0f, "%.2f");
		ImGui::ColorEdit3("Box Color", reinterpret_cast<float*>(&orbitBoxColor));

		ImGui::Separator();

		// Actions
		if (ImGui::Button("Respawn Orbiters"))
		{
			CreateOrbitingBoxes(wnd.Gfx(), physics, scene); // assumes these exist
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Orbiters"))
		{
			orbiters.clear();
		}

		ImGui::End();
	}



	wnd.Gfx().EndFrame();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto Preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;

		if (ImGui::BeginCombo("Box Number", Preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = comboBoxIndex ? (*comboBoxIndex == i) : false;
				if ( ImGui::Selectable( std::to_string( i ).c_str(), selected ) )
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
	for (auto i = boxControlIds.begin(); i != boxControlIds.end();)
	{
		if ( !boxes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
		{
			i = boxControlIds.erase(i);
		}
		else
		{
			i++;
		}
	}
}

void App::SpawnSimulationWindows() noexcept
{
	if (ImGui::Begin("Simulation of speed"))
	{
		ImGui::SliderFloat("Speed Factor", &SpeedFactor, 0.0f, 5.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Appplication Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Press 'I' to Disable/Enable Imgui");
		//if ( ImGui::Button("Reset") )
		//{
		//	CreateOrbitingBoxes(wnd.Gfx(), PhysXWrapper::Get()->GetgPhysics(), PhysXWrapper::Get()->GetgScene());
		//}
		for ( auto& o : orbiters)
		{
			ImGui::Text("box pos x = %.1f , y = %.1f, z = %.1f", o->GetPosition().x,
				o->GetPosition().y, o->GetPosition().z);
		}
	}
	ImGui::End();
}

void App::CreateOrbitingBoxes(Graphics& gfx, PxPhysics* physics, PxScene* scene)
{

	orbiters.clear();

	for (int i = 0; i < numOrbitingBoxes; ++i)
	{
		float angle = (PxTwoPi * i) / numOrbitingBoxes;
		float x = orbitCenter.x + initialRadius * cosf(angle);
		float z = orbitCenter.z + initialRadius * sinf(angle);

		auto box = std::make_unique<Box>(gfx, physics, boxSize, boxSize, boxSize);
		box->SetPosition(x + distancernd(rngg), orbitCenter.y, z + distancernd(rngg));

		PxRigidDynamic* body = box->GetRigidBody();


		body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !orbitUseGravity);

		// Initial tangent velocity
		PxVec3 toCenter = orbitCenter - PxVec3(x, orbitCenter.y, z);
		PxVec3 tangentVel = PxVec3(-toCenter.z, 0.0f, toCenter.x).getNormalized() * orbitSpeed;
		body->setLinearVelocity(tangentVel);

		body->setAngularDamping(0.5f);
		body->setLinearDamping(0.0f);

		std::cout << "Initial pos: " << x << ", " << z << "\n";
		std::cout << "Velocity: " << tangentVel.x << ", " << tangentVel.z << "\n";

		orbiters.push_back(std::move(box));
	}
}

void App::UpdateOrbiterForces()
{
	if (applyForceContinuously)
	{
		for (auto& box : orbiters)
		{
			auto* body = box->GetRigidBody();
			PxVec3 pos = body->getGlobalPose().p;
			PxVec3 toCenter = orbitCenter - pos;
			float dist = toCenter.magnitude();

			if (dist > 0.01f)
			{
				PxVec3 force = toCenter.getNormalized() * (forceScale / dist);
				body->addForce(force, PxForceMode::eACCELERATION);
			}
		}
	}

}
void App::DrawOrbitDebug(Graphics& gfx)
{
	// 1. Draw center marker
	DebugDraw::DrawSphere(gfx, orbitCenter, 0.2f, { 1.0f, 1.0f, 0.0f }); // Yellow sphere

	// 2. Draw circular orbit ring
	const int segments = 64;
	std::vector<DirectX::XMFLOAT3> circlePoints;
	for (int i = 0; i <= segments; ++i)
	{
		float angle = DirectX::XM_2PI * i / segments;
		float x = orbitCenter.x + initialRadius * cosf(angle);
		float z = orbitCenter.z + initialRadius * sinf(angle);
		float y = orbitCenter.y;
		circlePoints.push_back({ x, y, z });
	}

	for (int i = 1; i < circlePoints.size(); ++i)
	{
		DebugDraw::DrawLine(gfx, circlePoints[i - 1], circlePoints[i], { 0.0f, 1.0f, 1.0f }); // Cyan circle
	}

	// 3. Optionally: draw lines from orbiters to center
	for (auto& box : orbiters)
	{
		DirectX::XMFLOAT3 p = box->GetPosition();
		DebugDraw::DrawLine(gfx, p, orbitCenter, { 1.0f, 0.0f, 0.0f }); // Red vectors
	}
}



