#include "App.h"
#include "imgui/imgui.h"
#include "GDIPlusManager.h"
#include "AssImpTest.h"
#include <DirectXMath.h>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace dx = DirectX;
GDIPlusManager gdi;

App::App()
	:
	wnd(800, 600, "happy window"),
	light( wnd.Gfx(), 0.5f)
{
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
	wnd.Gfx().BeginFrame(color);
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

	const auto transform = dx::XMMatrixRotationRollPitchYaw( pos.roll, pos.pitch, pos.yaw ) *
		dx::XMMatrixTranslation( pos.x, pos.y - 10.0f, pos.z + 6.0f);
	nano.Draw( wnd.Gfx(), transform );
	light.Draw( wnd.Gfx() );

	// ImGui Windows
	SpawnSimulationWindows();
	SpawnModelWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindows() noexcept
{
	if (ImGui::Begin("Simulation of speed"))
	{
		ImGui::SliderFloat("Speed Factor", &SpeedFactor, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Appplication Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Press 'I' to Disable/Enable Imgui");
		if (ImGui::Button("Reset"))
		{
			SpeedFactor = 0.4f;
		}
		if (ImGui::Button("RESET_ALL"))
		{
			cam.Reset();
			light.Reset();
			SpeedFactor = 0.4f;
		}
	}
	ImGui::End();
}

void App::SpawnModelWindow()
{
	if (ImGui::Begin("Model"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat( "X", &pos.x, -20.0f, 20.0f );
		ImGui::SliderFloat( "Y", &pos.y, -20.0f, 20.0f );
		ImGui::SliderFloat( "Z", &pos.z, -20.0f, 20.0f );

		ImGui::Text("Orientation");
		ImGui::SliderAngle( "Roll", &pos.roll, -180.0f, 180.0f );
		ImGui::SliderAngle( "Pitch", &pos.pitch, -180.0f, 180.0f );
		ImGui::SliderAngle( "Yaw", &pos.yaw, -180.0f, 180.0f );
		
		
		if (ImGui::Button("Reset"))
		{
			pos.roll = 0.0f;
			pos.pitch = 0.0f;
			pos.yaw = 0.0f;
			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;
		}
	}
	ImGui::End();
}
