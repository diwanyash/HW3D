#pragma once
#include "Bindable.h"
#include "ConstantBuffers.h"
#include <DirectXMath.h>
#include "Drawable.h"

class TransformCBuff : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
public:
	TransformCBuff( Graphics& gfx, const Drawable& parent);
	void Bind( Graphics& gfx ) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable& parent;
};