#include "Graphics.h"
#include "dxerr.h"
#include "d3dcompiler.h"
#include <wrl.h>
#include <sstream>
#include <cassert>

// namespaces
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphics::Graphics(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = WIDTH;
	sd.BufferDesc.Height = HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferCount = 1;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	// handle to backbuffer
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// Set render creation
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	// create VP
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = WIDTH;
	vp.Height = HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;


	// bind viewport
	pContext->RSSetViewports(1u, &vp);


	// create Texture2D
	D3D11_TEXTURE2D_DESC Tex2Desc;
	Tex2Desc.Width = WIDTH;
	Tex2Desc.Height = HEIGHT;
	Tex2Desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Tex2Desc.Usage = D3D11_USAGE_DYNAMIC;
	Tex2Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Tex2Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Tex2Desc.SampleDesc.Count = 1;
	Tex2Desc.SampleDesc.Quality = 0;
	Tex2Desc.MipLevels = 1;
	Tex2Desc.ArraySize = 1;
	Tex2Desc.MiscFlags = 0;

	GFX_THROW_INFO(pDevice->CreateTexture2D(&Tex2Desc,nullptr, &pTextureBuffer ));


	// create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Format = Tex2Desc.Format;
	SrvDesc.Texture2D.MipLevels = 1;
	//create srv
	GFX_THROW_INFO(pDevice->CreateShaderResourceView(pTextureBuffer.Get(), &SrvDesc, &pShaderResourceView));

	//create pixel shader
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));


	//create vertex shader
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));


	// create VertexBuffer
	const Vertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};


	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vbsd = {};
	vbsd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &vbsd, &pVertexBuffer));

	// create InputLayout


	wrl::ComPtr<ID3DBlob> pBlob2;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob2));
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	auto casjc = pBlob2->GetBufferPointer();
	auto casjfc = pBlob2->GetBufferSize();
	GFX_THROW_INFO(pDevice->CreateInputLayout(ied,2u,pBlob2->GetBufferPointer(),pBlob2->GetBufferSize(),&pInputLayout));
	
	// sample desc
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	GFX_THROW_INFO(pDevice->CreateSamplerState(&sampDesc, &pSamplerState));


	// buffer init
	pBufferS = reinterpret_cast<Color*>(
		_aligned_malloc(sizeof(Color) * WIDTH * HEIGHT, 16u));
	
}

Graphics::~Graphics()
{
	if (pBufferS)
	{
		_aligned_free(pBufferS);
		pBufferS = nullptr;
	}

	if (pContext)
	{
		pContext->ClearState();
	}
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	GFX_THROW_INFO(pContext->Map(pTextureBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &MappedTextureBuffer));

	// copying
	Color* pDst = reinterpret_cast<Color*>(MappedTextureBuffer.pData);
	const size_t dPitch = MappedTextureBuffer.RowPitch / sizeof(Color);
	const size_t sPitch = WIDTH;
	const size_t RowBytes = sPitch * sizeof(Color);

	for (size_t y = 0u; y < HEIGHT; y++)
	{
		memcpy(&pDst[y * dPitch], &pBufferS[y * sPitch], RowBytes);
	}

	// unmap
	pContext->Unmap(pTextureBuffer.Get(), 0u);


	//////// create InputLayout
	
	//extra 
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	auto casjc = pBlob->GetBufferPointer();
	auto casjfc = pBlob->GetBufferSize();
	GFX_THROW_INFO(pDevice->CreateInputLayout(ied, 2u, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));

	// render
	GFX_THROW_INFO_ONLY(pContext->IASetInputLayout(pInputLayout.Get()));
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	pContext->PSSetShaderResources(0u, 1u, pShaderResourceView.GetAddressOf());
	pContext->PSSetSamplers(0u, 1u, pSamplerState.GetAddressOf());
	GFX_THROW_INFO_ONLY(pContext->Draw(6u, 0u));

	// flip
	if (FAILED(hr = pSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::SetPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < int(WIDTH));
	assert(y >= 0);
	assert(y < int(HEIGHT));
	pBufferS[WIDTH * y + x] = c;
}

// Graphics exception stuff
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Dead Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Dead Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Dead Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}