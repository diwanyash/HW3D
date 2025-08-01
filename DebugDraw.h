#pragma once
#include <DirectXMath.h>
#include "Graphics.h"

namespace DebugDraw
{
	void DrawLine(Graphics& gfx, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& color);
	void DrawSphere(Graphics& gfx, const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT3& color);
}
