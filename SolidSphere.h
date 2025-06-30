#pragma once
#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere( Graphics& gfx, float radius = 0.5f );
	void Update(float dt) noexcept override;
	void SetPos( DirectX::XMFLOAT3 pos );
	DirectX::XMMATRIX GetTransformXM() const noexcept;
private:
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };

};