#include "TransformCBuff.h"

TransformCBuff::TransformCBuff(Graphics& gfx, const Drawable& parent)
	:
	vcbuf( gfx ),
	parent( parent )
{}

void TransformCBuff::Bind(Graphics & gfx) noexcept
{
	vcbuf.Update(gfx, DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection() ));
	vcbuf.Bind(gfx);
}

