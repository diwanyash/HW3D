#include "Melon.h"
#include "Sphere.h"
#include "DirectXMath.h"
#include "BindableBase.h"

Melon::Melon(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_real_distribution<float>& bdist,
	std::uniform_int_distribution<int>& longdist,
	std::uniform_int_distribution<int>& latdist)
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

	if ( !IsStaticInitialized())
	{


		auto pvs = std::make_unique<VertexShader>( gfx, L"Shaderm\\ColorIndexVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\ColorIndexPS.cso"));



		struct PixelShaderConstant
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}face_color[9];
		};

		const PixelShaderConstant cb2
		{
			{				
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{1.0f,0.0f,1.0f},
				{1.0f,1.0f,1.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{0.75f,0.75f,0.75f},
				{0.5f,1.0f,0.0f},
				{0.1f,0.3f,1.0f}
			}
		};

		AddStaticBind( std::make_unique<PixelConstantBuffer<PixelShaderConstant>>( gfx, cb2) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}, 
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) );
	}


	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));

	dx::XMStoreFloat3x3( &mt, dx::XMMatrixScaling( 1.0f, 1.0f, 1.2f));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

	AddBind( std::make_unique<TransformCBuff>( gfx, *this ) );

}

void Melon::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;

	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
