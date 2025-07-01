#include "TransformCBuff.h"

TransformCBuff::TransformCBuff(Graphics& gfx, const Drawable& parent, UINT slot = 0u)
	:
	parent( parent )
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx, slot );
	}
}

void TransformCBuff::Bind(Graphics & gfx) noexcept
{
	const auto model = parent.GetTransformXM();
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};
	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformCBuff::Transforms>> TransformCBuff::pVcbuf;