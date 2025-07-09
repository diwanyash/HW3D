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
	wnd(1280, 720, "happy window"),
	light( wnd.Gfx(), 0.5f)
{
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 1000.0f));
	wnd.DisableCursor();
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

	nano.Draw( wnd.Gfx());
	light.Draw( wnd.Gfx() );

	// ImGui Windows
	SpawnSimulationWindows();
	nano.ShowWindow("NanoSuit");
	ShowRawInputWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	wnd.Gfx().EndFrame();
}

void App::ShowRawInputWindow()
{
	while ( const auto d = wnd.mouse.ReadRawDelta() )
	{
		x += d->x;
		y += d->y;
	}
	if ( ImGui::Begin( "Raw Input" ))
	{
		ImGui::SetWindowSize(ImVec2(100, 50));
		ImGui::Text( "Tally (%d, %d)",x, y);
	}
	ImGui::End();
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
			nano.Reset();
		}
	}
	ImGui::End();
}
