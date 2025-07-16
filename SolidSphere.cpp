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
	const auto geometryTag = "$sphere." + std::to_string( radius );
	AddBind( VertexBuffer::Resolve(gfx, geometryTag ,model.vertices));
	AddBind( IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "Shaderm\\SolidColorVS.cso");
	auto pvsbc = static_cast<VertexShader&>(*pvs).GetByteCode();
	AddBind(std::move(pvs));

	AddBind( PixelShader::Resolve(gfx, "Shaderm\\SolidColorPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind( PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst));


	AddBind( InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));////////////// FIX IT

	AddBind( Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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




