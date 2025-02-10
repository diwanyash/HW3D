#pragma once
#include "DeadWin.h"
#include <d3d11.h>

class Graphics {
public:
	Graphics( HWND hwnd );
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = {red,green,blue,1.0f};
		pContext->ClearRenderTargetView(pTarget, color);
	}
private:
	ID3D11Device* pDevice;
	IDXGISwapChain* pSwap;
	ID3D11DeviceContext* pContext;
	ID3D11RenderTargetView* pTarget;
};