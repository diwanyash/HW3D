#include "App.h"
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Sheet.h"
#include "Drawable.h"
#include <memory>
#include "imgui/imgui.h"
#include "Surface.h"
#include "GDIPlusManager.h"

GDIPlusManager gdi;

App::App()
	:
	wnd(800, 600, "happy window")
{
	struct Vertex{

		DirectX::XMFLOAT3 pos;

	};

	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch ( typedist( rng ) )
			{
			case 0:
				return std::make_unique<Pyramid>(
				gfx, rng, adist, ddist, odist, rdist);
				break;
			case 1:
				return std::make_unique<Melon>(
				gfx, rng, adist, ddist, odist, rdist, bdist, longdiv, latdiv);
				break;
			case 2:
				return std::make_unique<Box>(
				gfx, rng, adist, ddist, odist, rdist, bdist);
				break;
			case 3:
				return std::make_unique<Sheet>(
				gfx, rng, adist, ddist, odist, rdist );
				break;
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
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 1.0f, 3.0f };
		std::uniform_int_distribution<int> latdiv{ 6, 20 };
		std::uniform_int_distribution<int> longdiv{ 12, 40 };
		std::uniform_int_distribution<int> typedist{ 2, 3 };
	};

	Factory factory( wnd.Gfx() );
	drawable.reserve( nDrawables );
	std::generate_n( std::back_inserter(drawable), nDrawables, factory );

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

	for (auto& d : drawable)
	{
		d->Update(dt);
		d->Draw(wnd.Gfx());
	}	

	static char Buffer[1024];
	if (ImGui::Begin("Simulation of speed"))
	{
 		ImGui::SliderFloat("Speed Factor", &SpeedFactor, 0.0f, 5.0f);
		ImGui::Text("Appplication Average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText("Some Text", Buffer, sizeof(Buffer));
	}

	ImGui::End();

	cam.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
