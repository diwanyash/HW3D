#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"

Box::Box(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist)
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
	if( !IsStaticInitialized() )
	{
		struct Vertex {
			struct
			{
				float x;
				float y;
				float z;
			}pos;
		};

		const std::vector<Vertex> vertices =
		{
			// front
			{-1.0f,-1.0f, -1.0   }, //0
			{-1.0f, 1.0f, -1.0    }, //1
			{ 1.0f, 1.0f, -1.0     }, //2
			{ 1.0f,-1.0f, -1.0    }, //3
			// back			 
			{-1.0f,-1.0f,  1.0 }, //4
			{-1.0f, 1.0f,  1.0  },  //5
			{ 1.0f, 1.0f,  1.0   },  //6
			{ 1.0f,-1.0f,  1.0  }, //7
		};

		// vretex buffer
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		// bind vertex buffer
		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		// bind pixel shader
		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));


		// create index buffer
		const std::vector<unsigned short> indices =
		{
			0,1,3,	3,1,2, //front
			7,3,2,	7,2,6, //left
			4,5,1,	4,1,0, //right
			1,5,2,	2,5,6, //top 
			7,6,4,	4,6,5, //back
			4,0,7,	7,0,3, //bottom
		};

		// bind index buffer 
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		// create pixel const buffer
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}face_color[6];
		};

		const ConstantBuffer2 cb2
		{
			{
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{1.0f,0.0f,1.0f},
				{1.0f,1.0f,1.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
			}
		};

		// bind constant pixel buffer
		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantBuffer2>>(gfx, cb2));

		// create input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
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
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f , 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f) ;
}
