#pragma once
#include "DrawableBase.h"
#include "DeadMath.h"
#include <physx/PxPhysicsAPI.h>

using namespace physx;

template<class T>
class BaseObject : public DrawableBase<T>
{
public:
	BaseObject(
		Graphics& gfx, std::mt19937& rng,
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
		chi(adist(rng)),
		transform(DirectX::XMMatrixIdentity())
	{
	}

	void Update(float dt) noexcept override
	{
		roll = wrap_angle(roll + droll * dt);
		pitch = wrap_angle(pitch + dpitch * dt);
		yaw = wrap_angle(yaw + dyaw * dt);
		theta = wrap_angle(theta + dtheta * dt);
		phi = wrap_angle(phi + dphi * dt);
		chi = wrap_angle(chi + dchi * dt);

		if (pPhysicsBody)
		{
			PxTransform t = pPhysicsBody->getGlobalPose();
			SetTransform(DirectX::XMMatrixTranslation(t.p.x, t.p.y, t.p.z));
			SetRotationFromQuaternion(t.q);
		}
	}

	void InitRigidDynamic(PxPhysics* physics, const PxVec3& pos = PxVec3(1.0f, 1.0f, 5.0f))
	{
		PxTransform startTransform(pos);
		pPhysicsBody = physics->createRigidDynamic(startTransform);
		if (!pPhysicsBody)
			throw std::runtime_error("Failed to create rigid dynamic actor");
	}

	void SetTransform(DirectX::FXMMATRIX matrix)
	{
		transform = matrix;
	}

	void SetRotationFromQuaternion(const PxQuat& q)
	{
		using namespace DirectX;
		XMVECTOR quat = XMVectorSet(q.x, q.y, q.z, q.w);
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(quat);
		XMStoreFloat3x3(&mt, rotMatrix);
	}

	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return transform;
	}

	physx::PxRigidDynamic* GetRigidBody() const noexcept
	{
		return pPhysicsBody;
	}

protected:
	// Position
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// Rotation speed
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;

	DirectX::XMMATRIX transform;
	DirectX::XMFLOAT3X3 mt;

	physx::PxRigidDynamic* pPhysicsBody = nullptr;
};
