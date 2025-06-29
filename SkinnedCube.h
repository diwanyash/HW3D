#pragma once
#include "DrawableBase.h"

class SkinnedCube : public DrawableBase<SkinnedCube>
{
public:
	SkinnedCube(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist
	);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept;
private:
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


	DirectX::XMFLOAT3X3 mt;
};