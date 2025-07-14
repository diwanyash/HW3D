#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

namespace bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf);
		void Bind(Graphics& gfx) noexcept override;
	protected:
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
}