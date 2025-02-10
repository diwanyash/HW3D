#pragma once
#include "DeadWin.h"
#include "DeadException.h"
#include <d3d11.h>

class Graphics {
public:
	class Exception : public Deadexception
	{
		using Deadexception::Deadexception;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	};
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