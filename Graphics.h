#pragma once
#include "DeadWin.h"
#include "DeadException.h"
#include <d3d11.h>
#include <vector>
#include "wrl.h"
#include "DxgiInfoManager.h"
#include "d3dcompiler.h"
#include <DirectXMath.h>

class Graphics {
	friend class Bindable;
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
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
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
	void BeginFrame(float red, float green, float blue) noexcept;
	void BeginFrame(float color[]) noexcept
	{
		BeginFrame( color[0], color[1], color[2] );
	}
	void DrawIndexed( UINT count );
	void SetProjection( DirectX::FXMMATRIX proj )noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera( DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;
	void DrawTestTriangle(int i_vp, float angle, float x, float y, float z);
private:
	DirectX::XMMATRIX Projection;
	DirectX::XMMATRIX Camera;
	bool ImguiEnabled = true;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};