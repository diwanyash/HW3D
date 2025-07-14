#include "SolidSphere.h"
#include "Sphere.h"
#include "DirectXMath.h"
#include "Vertex.h"
#include "BindableBase.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	auto pvs = std::make_shared<VertexShader>(gfx, "Shaderm\\SolidColorVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, "Shaderm\\SolidColorPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));


	AddBind(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

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




