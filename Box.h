#pragma once
#include "BaseObject.h"
#include "ConstantBuffers.h"

class Box : public BaseObject<Box>
{
public:
	Box(Graphics& gfx,std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material
	);
	Box(Graphics& gfx, physx::PxPhysics* physics, float width, float height, float depth);
	DirectX::XMMATRIX GetTransformXM() const noexcept;
	bool SpawnControlWindow(int id, Graphics& gfx)noexcept;
	// In Box.h
	void SetPosition(float x, float y, float z);
private:
	void SyncMaterial( Graphics& gfx) noexcept(!IS_DEBUG);
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specular_intensity = 0.6f;
		float specular_power = 30.0f;
		float padding[3];
	}materialConstants;
	using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;
private:
	DirectX::XMFLOAT3X3 mt;
	std::mt19937 dummy;
	std::uniform_real_distribution<float> ff;
};