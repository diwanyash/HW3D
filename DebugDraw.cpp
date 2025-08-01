#pragma once
#include "Drawable.h"
#include "SolidSphere.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "Topology.h"
#include "TransformCBuff.h"

namespace DebugDraw
{
	class Line : public Drawable
	{
	public:
		Line(Graphics& gfx, const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, const DirectX::XMFLOAT3& color)
		{
			namespace dx = DirectX;

			struct Vertex
			{
				dx::XMFLOAT3 pos;
				dx::XMFLOAT3 color;
			};

			const Vertex vertices[] = {
				{ a, color },
				{ b, color }
			};

			const std::vector<unsigned short> indices = { 0, 1 };



			AddBind(std::make_shared<VertexBuffer>(gfx, vertices));

			AddBind(std::make_shared<IndexBuffer>(gfx, indices));

			auto pVS = std::make_shared<VertexShader>(gfx, "ColorVS.cso");
			auto pVSBC = pVS->GetBytecode();
			AddBind(pVS);

			AddBind(std::make_shared<PixelShader>(gfx, "ColorPS.cso"));

			AddBind(std::make_shared<InputLayout>(
				gfx,
				std::vector<D3D11_INPUT_ELEMENT_DESC>{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			},
				pVSBC
			));

			AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));
			AddBind(std::make_shared<TransformCBuff>(gfx, *this));
		}

		DirectX::XMMATRIX GetTransformXM() const noexcept override
		{
			return DirectX::XMMatrixIdentity();
		}
	};
}
