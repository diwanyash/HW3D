#pragma once
#include "Bindable.h"
#include <memory>

class Surface;

namespace bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& path, UINT slot);
		static std::string GenerateUID( const std::string& path, UINT slot );
		std::string GetUID() const noexcept override;
	private:
		unsigned int slot;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::string path;
	};
}