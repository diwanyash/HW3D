#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "InputLayout.h"

namespace bind
{
	Texture::Texture(Graphics& gfx, const std::string& path, UINT slot)
		:
		slot(slot)
	{
		INFOMAN(gfx);

		const auto s = Surface::FromFile( path );

		D3D11_TEXTURE2D_DESC texturedesc = {};
		texturedesc.Width = s.GetWidth();
		texturedesc.Height = s.GetHeight();
		texturedesc.MipLevels = 1;
		texturedesc.ArraySize = 1;
		texturedesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texturedesc.SampleDesc.Count = 1;
		texturedesc.SampleDesc.Quality = 0;
		texturedesc.Usage = D3D11_USAGE_DEFAULT;
		texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texturedesc.CPUAccessFlags = 0;
		texturedesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = s.GetBufferPtr();
		sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texturedesc, &sd, &pTexture));


		// view desc
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texturedesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}
	std::shared_ptr<Bindable> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>( gfx, path, slot );
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string( slot ); 
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID( path, slot );
	}
}







