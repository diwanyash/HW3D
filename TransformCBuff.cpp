#include "TransformCBuff.h"

TransformCBuff::TransformCBuff(Graphics& gfx, const Drawable& parent)
	:
	parent( parent )
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformCBuff::Bind(Graphics & gfx) noexcept
{
	pVcbuf->Update(gfx, 
		DirectX::XMMatrixTranspose(parent.GetTransformXM() *
		gfx.GetCamera() *
		gfx.GetProjection() ));
	pVcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuff::pVcbuf;