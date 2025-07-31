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

namespace dx = DirectX;
GDIPlusManager gdi;

App::App()
	:
	wnd(800, 600, "happy window"),
	light( wnd.Gfx(), 0.5f)
{

	CreateOrbitingBoxes(wnd.Gfx(), PhysXWrapper::Get()->GetgPhysics(), PhysXWrapper::Get()->GetgScene());
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

	Factory factory( wnd.Gfx() );
	drawable.reserve( nDrawables );
	std::generate_n( std::back_inserter(drawable), nDrawables, factory );

	for ( auto& pd : drawable)
	{
		if ( auto pb = dynamic_cast<Box*>(pd.get()) ) 
		{
			boxes.push_back(pb);
		}
	}


	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 1000.0f));
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
	light.Draw( wnd.Gfx() );

	// ImGui Windows
	SpawnSimulationWindows();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();
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
	}
	ImGui::End();
}

void App::CreateOrbitingBoxes(Graphics& gfx, PxPhysics* physics, PxScene* scene)
{
	const int numBoxes = 20;
	const float radius = 10.0f;
	const float boxSize = 1.0f;
	const float orbitSpeed = 10.0f;

	orbiters.clear();

	for (int i = 0; i < numBoxes; ++i)
	{
		float angle = (PxTwoPi * i) / numBoxes;
		float x = orbitCenter.x + radius * cosf(angle);
		float z = orbitCenter.z + radius * sinf(angle);

		auto box = std::make_unique<Box>(gfx, physics, boxSize, boxSize, boxSize);
		box->SetPosition(x, orbitCenter.y, z);

		PxRigidDynamic* body = box->GetRigidBody();

		// Tangent direction to orbit circle
		PxVec3 toCenter = orbitCenter - PxVec3(x, orbitCenter.y, z);
		PxVec3 tangentVel = PxVec3(-toCenter.z, 0.0f, toCenter.x).getNormalized() * orbitSpeed;

		body->setLinearVelocity(tangentVel);
		body->setAngularDamping(0.5f); // Reduce spin
		body->setLinearDamping(0.0f);  // Let it orbit freely

		scene->addActor(*body);
		orbiters.push_back(std::move(box));
	}
}

void App::UpdateOrbiterForces()
{
	for (auto& orbiter : orbiters)
	{
		PxRigidDynamic* body = orbiter->GetRigidBody();
		PxVec3 pos = body->getGlobalPose().p;
		PxVec3 toCenter = orbitCenter - pos;
		float distSq = toCenter.magnitudeSquared();

		if (distSq > 0.001f)
		{
			// Apply force using an inverse-square falloff
			PxVec3 force = toCenter.getNormalized() * (500.0f / distSq);
			body->addForce(force, PxForceMode::eACCELERATION);
		}
	}
}

