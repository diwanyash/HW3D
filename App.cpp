#include "App.h"
#include "imgui/imgui.h"
#include "GDIPlusManager.h"
#include "AssImpTest.h"
#include <DirectXMath.h>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "BindableCodex.h"

namespace dx = DirectX;
GDIPlusManager gdi;

App::App()
	:
	wnd(1280, 720, "happy window"),
	light( wnd.Gfx(), 0.5f),
	plane( wnd.Gfx(), 4.0f )
{
	plane.SetPos( { 1.0f, 15.0f, -2.0f } );
	wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 1000.0f));
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
	const auto dt = ft.Mark() * SpeedFactor;

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
		else if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
		{
			if (wnd.IsCursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
		}
		wnd.kbd.Flush();
	}

	if ( !wnd.IsCursorEnabled() )
	{
		if ( wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f, 0.0f, dt });
		}
		if ( wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if( wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if ( wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt, 0.0f, 0.0f });
		}
		if ( wnd.kbd.KeyIsPressed( VK_SPACE ))
		{
			cam.Translate({ 0.0f, dt, 0.0f });
		}
		if ( wnd.kbd.KeyIsPressed( VK_CONTROL ))
		{
			cam.Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.IsCursorEnabled())
		{
			cam.Rotate(delta->x, delta->y);
		}
	}

	nano.Draw( wnd.Gfx());
	light.Draw( wnd.Gfx() );
	plane.Draw( wnd.Gfx() );

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
	if ( ImGui::Begin( "Raw Input", NULL, ImGuiWindowFlags_NoResize ))
	{
		ImGui::SetWindowSize(ImVec2(140, 70));
		ImGui::Text( "Tally (%d, %d)",x, y);
		ImGui::Text("Cursor %s", wnd.IsCursorEnabled() ? "Enabled" : "Disabled");
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
