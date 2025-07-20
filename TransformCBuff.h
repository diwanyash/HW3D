#pragma once
#include "Bindable.h"
#include "ConstantBuffers.h"
#include <DirectXMath.h>
#include "Drawable.h"

namespace bind
{
	class TransformCBuff : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCBuff(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl( Graphics& gfx, const Transforms& tf ) noexcept;
		Transforms GetTransforms( Graphics& gfx ) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}