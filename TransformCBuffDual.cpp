#include "TransformCBuffDual.h"

namespace bind
{
	TransformCBuffDual::TransformCBuffDual(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		:
		TransformCBuff( gfx, parent, slotV )
	{
		if (!pPcbuf)
		{
			pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}

	void TransformCBuffDual::Bind(Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms( gfx );
		TransformCBuff::UpdateBindImpl( gfx, tf );
		UpdateBindImpl( gfx, tf );
	}

	void TransformCBuffDual::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		pPcbuf->Update( gfx, tf );
		pPcbuf->Bind( gfx );
	}


	std::unique_ptr<PixelConstantBuffer<TransformCBuff::Transforms>> TransformCBuffDual::pPcbuf;
}