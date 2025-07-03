#include "Box.h"
#include "Cube.h"
#include "Plane.h"
#include "BaseObject.h"
#include "BindableBase.h"
#include "DirectXMath.h"
#include "GraphicsThrowMacros.h"

Box::Box(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 Material)
	:
	BaseObject( gfx, rng, adist, ddist, odist, rdist)
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

	struct PSMaterialConstant
	{
		alignas(16) dx::XMFLOAT3 color;
		float specular_intensity = 0.6f;
		float specular_power = 30.0f;
		float padding[2];
	}colorConst;
	colorConst.color = Material;

	AddBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u ));

	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))); 

}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) * BaseObject::GetTransformXM();
}
