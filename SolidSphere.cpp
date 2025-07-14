#include "SolidSphere.h"
#include "Sphere.h"
#include "DirectXMath.h"
#include "BindableBase.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace bind;
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = Sphere::Make<Vertex>();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	auto pvs = std::make_shared<VertexShader>(gfx, L"Shaderm\\SolidColorVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"Shaderm\\SolidColorPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	AddBind(std::make_shared<InputLayout>(gfx, ied, pvsbc));

	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCBuff>(gfx, *this));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos)
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}




