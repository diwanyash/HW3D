#include "Box.h"
#include "Cube.h"
#include "Plane.h"
#include "BindableBase.h"
#include "DirectXMath.h"
#include "GraphicsThrowMacros.h"

Box::Box(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dtheta(odist(rng)),
	dphi(odist(rng)),
	dchi(odist(rng)),
	theta(adist(rng)),
	phi(adist(rng)),
	chi(adist(rng))
{
	namespace dx = DirectX;

	if( !IsStaticInitialized() )
	{
		struct Vertex {
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		
		//set normals automatically
		model.SetNormalsIndependentFlat();
		// vretex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx,model.vertices));

		// bind vertex buffer
		auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		// bind pixel shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\PhongPS.cso"));

		// bind index buffer 
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		// create input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		// bind input layout
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		// bind topology
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	// bind transform constant buffer ( TransformCBuf )
	AddBind(std::make_unique<TransformCBuff>(gfx, *this));

	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))); 
}

void Box::Update(float dt) noexcept
{
	roll +=  droll  * dt;
	pitch += dpitch * dt;
	yaw +=   dyaw   * dt;
	theta += dtheta * dt;
	phi +=   dphi   * dt;
	chi +=   dchi   * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;

	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f , 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
