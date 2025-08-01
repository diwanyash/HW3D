#include "Box.h"
#include "Cube.h"
#include "Plane.h"
#include "imgui/imgui.h"
#include "BaseObject.h"
#include "BindableBase.h"
#include "DirectXMath.h"
#include "GraphicsThrowMacros.h"
#include "PhysXWrapper.h"

Box::Box(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 Material)
	:
	BaseObject( gfx, rng, adist, ddist, odist, rdist),
	dummy(rng),
	ff(bdist)
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

	materialConstants.color = Material;

	AddBind( std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u ));

	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))); 

	pPhysicsBody = PhysXWrapper::Get()->GetgPhysics()->createRigidDynamic(PxTransform(1.0f, 1.0f, 1.0f));
	physx::PxVec3 halfExtent = physx::PxVec3(0.5f, 0.5f, 0.5f);  // Half of cube size
	physx::PxMaterial* material = PhysXWrapper::Get()->GetgMaterial();  // Assuming you add this method
	physx::PxShape* shape = PhysXWrapper::Get()->GetPhysics()->createShape(
		physx::PxBoxGeometry(halfExtent),
		*PhysXWrapper::Get()->GetgMaterial()
	); 
	pPhysicsBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*pPhysicsBody, 10.0f);
	PhysXWrapper::Get()->GetgScene()->addActor(*pPhysicsBody);
	//pPhysicsBody->setLinearVelocity(PxVec3(0.0f, 0.1f, 0.0f));
	pPhysicsBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);    // no gravity
	pPhysicsBody->setLinearVelocity(PxVec3(0.0f));                       // zero velocity
	pPhysicsBody->setAngularVelocity(PxVec3(0.0f));                      // no spin
	pPhysicsBody->setLinearDamping(100.0f);                              // kill all linear motion
	pPhysicsBody->setAngularDamping(100.0f);                             // kill all angular motion
	pPhysicsBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); // ensure dynamic
	pPhysicsBody->clearForce();                                          // no residual forces
	pPhysicsBody->clearTorque();

}

Box::Box(Graphics& gfx, physx::PxPhysics* physics, float width, float height, float depth)
	: BaseObject(gfx, dummy ,ff,ff,ff,ff)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex {
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\PhongPS.cso"));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuff>(gfx, *this));
	materialConstants.color = { 1.0f, 1.0f, 1.0f }; // Default white
	AddBind(std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u));

	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f,1.0f));

	// Create physics body
	pPhysicsBody = physics->createRigidDynamic(PxTransform(0.0f, 0.0f, 0.0f));
	physx::PxVec3 halfExtent(width / 2.0f, height / 2.0f, depth / 2.0f);
	physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(halfExtent), *PhysXWrapper::Get()->GetgMaterial());

	pPhysicsBody->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*pPhysicsBody, 10.0f);
	PhysXWrapper::Get()->GetgScene()->addActor(*pPhysicsBody);



	//pPhysicsBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);    // no gravity
	//pPhysicsBody->setLinearVelocity(PxVec3(0.0f));                       // zero velocity
	//pPhysicsBody->setAngularVelocity(PxVec3(0.0f));                      // no spin
	//pPhysicsBody->setLinearDamping(100.0f);                              // kill all linear motion
	//pPhysicsBody->setAngularDamping(100.0f);                             // kill all angular motion
	//pPhysicsBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); // ensure dynamic
	//pPhysicsBody->clearForce();                                          // no residual forces
	//pPhysicsBody->clearTorque();
}


DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) * BaseObject::GetTransformXM();
}

bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
{
	using namespace std::string_literals;

	bool Changed = false;
	bool open = true;
	if( ImGui::Begin( ("Box "s + std::to_string(id)).c_str(),&open))
	{
		ImGui::Text( " Material Properties ");
		const auto mc = ImGui::ColorEdit3( "Material Color",&materialConstants.color.x);
		const auto si = ImGui::SliderFloat("Specular Intensity", &materialConstants.specular_intensity, 0.05f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		const auto sp = ImGui::SliderFloat("Specular Power", &materialConstants.specular_power, 1.0f, 200.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		Changed = mc || si || sp;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if ( Changed )
	{
		SyncMaterial( gfx );
	}

	return open;
}

void Box::SetPosition(float x, float y, float z)
{
	if (pPhysicsBody)
	{
		PxTransform t(PxVec3(x, y, z));
		pPhysicsBody->setGlobalPose(t);
	}
}

void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert( pConstPS != nullptr );
	pConstPS->Update(gfx, materialConstants);
}
