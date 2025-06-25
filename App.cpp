#include "App.h"
#include "Box.h"
#include "Melon.h"
#include <memory>
#include "imgui/imgui.h"

App::App()
	:
	wnd( 800, 600, "happy window" )
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	std::uniform_real_distribution<float> bdist(1.0f, 3.0f);
	std::uniform_int_distribution<int> latdiv( 6, 12 );
	std::uniform_int_distribution<int> longdiv( 12, 24 );
	std::uniform_int_distribution<int> obj( 10, 50 );
	for (int i = obj(rng); i > 0; i--)
	{
		melons.push_back(std::make_unique<Melon>(wnd.Gfx(),
			rng, adist, ddist, odist, rdist, bdist, longdiv, latdiv));
	}
	for (int i = obj(rng); i > 0; i--)
	{
		boxes.push_back(std::make_unique<Box>(wnd.Gfx(),
			rng, adist, ddist, odist, rdist, bdist));
	}
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 1000.0f));
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
	wnd.Gfx().SetCamera(cam.GetMatrix());


	if (wnd.kbd.KeyIsPressed(VK_SPACE))
	{
		wnd.Gfx().DisableImgui();
	}
	else
	{
		wnd.Gfx().EnableImgui();
	}

	for (auto& b : melons)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}	
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	static char Buffer[1024];
	if (ImGui::Begin("Simulation of speed"))
	{
 		ImGui::SliderFloat("Speed Factor", &SpeedFactor, 0.0f, 5.0f);
		ImGui::Text("Appplication Average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Boxes = %d , Melons = %d", (int)boxes.size(), (int)melons.size());
		ImGui::InputText("Some Text", Buffer, sizeof(Buffer));
	}

	ImGui::End();

	cam.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
