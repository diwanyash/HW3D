#include "Pyramid.h"
#include "Cone.h"
#include "DirectXMath.h"
#include "BindableBase.h"
#include <array>

Pyramid::Pyramid(Graphics& gfx, std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& tdist)
	:
	BaseObject( gfx, rng, adist, ddist, odist, rdist )
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
			std::array<char, 4> color;
			char padding;
		};   
		const auto Tesselation = tdist(rng);
		auto model = Cone::MakeTesselatedIndependentFaces<Vertex>( Tesselation );

		for ( auto& v : model.vertices )
		{
			v.color = { (char)10, (char)10, (char)255 };
		}
		for( int i = 0; i < Tesselation; i++ )
		{
			model.vertices[ i * 3 ].color = { (char)255, (char)10, (char)10 };
		}

		// model.vertices[0].color = { 255, 255, 0 };
		// model.vertices[1].color = { 255, 255, 0 };
		// model.vertices[2].color = { 255, 255, 0 };
		// model.vertices[3].color = { 255, 255, 0 };
		// model.vertices[4].color = { 255, 0, 0 };
		// model.vertices[5].color = { 0, 255, 0 };

		model.Transform(dx::XMMatrixScaling( 1.0f, 1.0f, 0.7f ));

		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\BlendedPhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\BlendedPhongPS.cso"));


		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float specular_intensity = 0.6f;
			float specular_power = 30.0f;
			float padding[2];
		}colorConst;

		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuff>(gfx, *this));

}
