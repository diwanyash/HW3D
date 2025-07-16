#pragma once
#include "DeadMath.h"
#include "Drawable.h"
#include <random>
#include <DirectXMath.h>

template<class T>
class BaseObject : public Drawable
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
		chi(adist(rng))
	{}
	//void Update(float dt) noexcept override
	//{
	//	roll  = wrap_angle(roll + droll * dt );
	//	pitch = wrap_angle(pitch + dpitch * dt);
	//	yaw	  = wrap_angle(yaw + dyaw * dt  );
	//	theta = wrap_angle(theta + dtheta * dt);
	//	phi   = wrap_angle(phi + dphi * dt  );
	//	chi   = wrap_angle(chi + dchi * dt  );
	//}
	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		namespace dx = DirectX;

		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
protected:	
	// pos
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed by d/t
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
};