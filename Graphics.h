#pragma once
#include "DeadWin.h"
#include "DeadException.h"
#include <d3d11.h>
#include <vector>
#include "wrl.h"
#include "DxgiInfoManager.h"
#include "Color.h"

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
	void ClearBuffer() noexcept
	{
		memset(pBufferS, 0u, sizeof(Color) * HEIGHT * WIDTH);
	}
	void SetPixel(int x, int y, Color c);
	void DrawTestTriangle();
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;
private:	
	struct Vertex {
				float x;
				float y;
				float z;
				float u;
				float v;
			 };
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	D3D11_MAPPED_SUBRESOURCE MappedTextureBuffer;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget; 
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTextureBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer > pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader; 
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader; 
private:
	Color* pBufferS = nullptr;
};