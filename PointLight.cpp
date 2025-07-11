#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& gfx, float radius)
	:
	mesh( gfx, radius ),
	cbuf( gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Light" ))
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X", &pcbuf.pos.x, -60.0f, 60.0f, "%.1f" );
		ImGui::SliderFloat( "Y", &pcbuf.pos.y, -60.0f, 60.0f, "%.1f" );
		ImGui::SliderFloat( "Z", &pcbuf.pos.z, -60.0f, 60.0f, "%.1f" );

		ImGui::Text("Intensirt/Color");
		ImGui::SliderFloat("Intensity", &pcbuf.diffuse_intensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Color", &pcbuf.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &pcbuf.ambient.x);
		
		ImGui::Text("FallOff");
		ImGui::SliderFloat("Constant", &pcbuf.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &pcbuf.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &pcbuf.attQuad, 0.0000001f, 10.0f, "%.7f", ImGuiSliderFlags_Logarithmic);

		if( ImGui::Button("Reset") )
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	pcbuf =
	{
	   { 1.5f , 14.0f, -4.5f },
	   { 0.005f, 0.005f, 0.005f },
	   { 1.0f, 1.0f, 1.0f },
	   1.0f,
	   1.0f,
	   0.045f,
	   0.0075f,
	};
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mesh.SetPos( pcbuf.pos );
	mesh.Draw( gfx );
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept
{
	auto dataCopy = pcbuf;
	const auto pos = DirectX::XMLoadFloat3( &pcbuf.pos );
	DirectX::XMStoreFloat3( &dataCopy.pos, DirectX::XMVector3Transform( pos, view ) );
	cbuf.Update(gfx, PointLightCBuf{ dataCopy });
	cbuf.Bind( gfx );
}




