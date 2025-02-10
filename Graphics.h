#pragma once
#include "DeadWin.h"
#include "DeadException.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"

class Graphics {
public:
	class Exception : public Deadexception
	{
		using Deadexception::Deadexception;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr,std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
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
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	ID3D11Device* pDevice;
	IDXGISwapChain* pSwap;
	ID3D11DeviceContext* pContext;
	ID3D11RenderTargetView* pTarget;
};