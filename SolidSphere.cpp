#include "SolidSphere.h"
#include "Sphere.h"
#include "DirectXMath.h"
#include "BindableBase.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	namespace dx = DirectX;

		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform( dx::XMMatrixScaling( radius, radius, radius ));
		AddBind(std::shared_ptr<VertexBuffer>(gfx, model.vertices));
		AddBind(std::shared_ptr<IndexBuffer>(gfx, model.indices));

		auto pvs = std::shared_ptr<VertexShader>(gfx, L"Shaderm\\SolidColorVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddBind(std::move(pvs));

		AddBind(std::shared_ptr<PixelShader>(gfx, L"Shaderm\\SolidColorPS.cso"));

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		AddBind(std::shared_ptr<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));///////////

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		};

		AddBind(std::shared_ptr<InputLayout>(gfx, ied, pvsbc));

		AddBind(std::shared_ptr<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));




	AddBind(std::shared_ptr<TransformCBuff>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept
{}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos)
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );
}




