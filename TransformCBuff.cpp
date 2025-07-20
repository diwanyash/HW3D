#include "TransformCBuff.h"
#include "TransformCBuffDual.h"

namespace bind
{
	TransformCBuff::TransformCBuff(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		parent(parent)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCBuff::Bind(Graphics& gfx) noexcept
	{
		UpdateBindImpl( gfx, GetTransforms( gfx ) );
	}

	void TransformCBuff::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	TransformCBuff::Transforms TransformCBuff::GetTransforms(Graphics& gfx) noexcept
	{
		const auto modelView = parent.GetTransformXM() * gfx.GetCamera();
		return{
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView *
				gfx.GetProjection()
			)
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCBuff::Transforms>> TransformCBuff::pVcbuf;

}