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
#include "SkinnedCube.h"
#include "Cylinder.h"

GDIPlusManager gdi;

App::App()
	:
	wnd(800, 600, "happy window"),
	light( wnd.Gfx(), 0.5f)
{
	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cdist(rng),  cdist(rng),  cdist(rng), };

			switch ( typedist( rng ) )
			{
			//case 0:
			//	return std::make_unique<Pyramid>(
			//	gfx, rng, adist, ddist, odist, rdist);
			//	break;
			case 1:
				return std::make_unique<Cylinder>(
				gfx, rng, adist, ddist, odist, rdist, bdist, tdist);
				break;
			case 2:
				return std::make_unique<Box>(
				gfx, rng, adist, ddist, odist, rdist, bdist, mat);
				break;
			//case 3:
			//	return std::make_unique<Sheet>(
			//	gfx, rng, adist, ddist, odist, rdist );
			//	break;
			//case 4:
			//	return std::make_unique<SkinnedCube>(
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
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 1.0f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f };
		std::uniform_int_distribution<int> latdiv{ 6, 20 };
		std::uniform_int_distribution<int> longdiv{ 12, 40 };
		std::uniform_int_distribution<int> typedist{ 1, 2 };
		std::uniform_int_distribution<int> tdist{ 3, 30 };
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

	static char Buffer[1024];
	if (ImGui::Begin("Simulation of speed"))
	{
 		ImGui::SliderFloat("Speed Factor", &SpeedFactor, 0.0f, 5.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("Appplication Average %.3f ms/frame (%.1f FPS)",1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Press 'I' to Disable/Enable Imgui");
	}

	ImGui::End();

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
