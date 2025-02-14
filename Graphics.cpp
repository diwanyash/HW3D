#include "Graphics.h"
#include "dxerr.h"
#include "d3dcompiler.h"
#include <wrl.h>
#include "DirectXMath.h"
#include <sstream>

// namespaces
namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

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
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

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

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));


	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.StencilEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> dsState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &dsState));

	// bind depth stencil state
	pContext->OMSetDepthStencilState(dsState.Get(), 1u);

	// Create texture2d for depth stencil
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC texds_desc = {};
	texds_desc.Width = 800u;
	texds_desc.Height = 600u;
	texds_desc.MipLevels = 1u;
	texds_desc.ArraySize = 1u;
	texds_desc.SampleDesc.Count = 1u;
	texds_desc.SampleDesc.Quality = 0u;
	texds_desc.Format = DXGI_FORMAT_D32_FLOAT;
	texds_desc.Usage = D3D11_USAGE_DEFAULT;
	texds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&texds_desc,nullptr,&pDepthStencil));

	// depth stencil view desc
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(),&dsvDesc,&pDSV));

	// bind render target to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(),pDSV.Get());
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
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

void Graphics::DrawTestTriangle(int i_vp, float angle, float x, float y, float z)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex {
		struct
		{
			float x;
			float y;
			float z;
		}pos;
	};

	// create vertex buffer
	const Vertex vertices[] =
	{
		// front
		{-1.0f,-1.0f, -1.0   }, //0
		{-1.0f, 1.0f, -1.0    }, //1
		{ 1.0f, 1.0f, -1.0     }, //2
		{ 1.0f,-1.0f, -1.0    }, //3
		// back			 
		{-1.0f,-1.0f,  1.0 }, //4
		{-1.0f, 1.0f,  1.0  },  //5
		{ 1.0f, 1.0f,  1.0   },  //6
		{ 1.0f,-1.0f,  1.0  }, //7
	};

	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	// bind vertex buffer
	const UINT pstride = sizeof(Vertex);
	const UINT poffset = 0u;
	pContext->IASetVertexBuffers(0u,1u,pVertexBuffer.GetAddressOf(), &pstride, &poffset);
	
	//// create index buffer
	const unsigned short indices[] =
	{
		0,1,3,	3,1,2, //front
		7,3,2,	7,2,6, //left
		4,5,1,	4,1,0, //right
		1,5,2,	2,5,6, //top 
		7,6,4,	4,6,5, //back
		4,0,7,	7,0,3, //bottom
	};

	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC bid = {};
	bid.Usage = D3D11_USAGE_DEFAULT;
	bid.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bid.CPUAccessFlags = 0u;
	bid.MiscFlags = 0u;
	bid.ByteWidth = sizeof(indices);
	bid.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA sid = {};
	sid.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bid, &sid, &pIndexBuffer));


	// bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT,0u);

	// create constant buffer ( vertex shader )
	struct ConstatntBuffer
	{
		dx::XMMATRIX transform;
	};

	const ConstatntBuffer cb
	{
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(angle)*
			dx::XMMatrixRotationX(angle)*
			dx::XMMatrixRotationY(angle)*
			dx::XMMatrixTranslation( x, y, z ) *
			dx::XMMatrixPerspectiveLH(1.0f,3.0f/4.0f,0.5f,10.0f)
		)
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC bcbd = {};
	bcbd.Usage = D3D11_USAGE_DYNAMIC;
	bcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bcbd.MiscFlags = 0u;
	bcbd.ByteWidth = sizeof(cb);
	bcbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA scbd = {};
	scbd.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bcbd, &scbd, &pConstantBuffer));

	// bind constant buffer
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// create constatnt (pixel) buffer
	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		}face_color[6];
	};

	const ConstantBuffer2 cb2
	{
		{
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{1.0f,0.0f,1.0f},
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{1.0f,1.0f,0.0f},
		}
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC bcbd2;
	bcbd2.Usage = D3D11_USAGE_DEFAULT;
	bcbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bcbd2.CPUAccessFlags = 0u;
	bcbd2.MiscFlags = 0u;
	bcbd2.ByteWidth = sizeof(cb2);
	bcbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA scbd2 = {};
	scbd2.pSysMem = &cb2;
	GFX_THROW_INFO(pDevice->CreateBuffer(&bcbd2, &scbd2, &pConstantBuffer2));

	// bind constant buffer
	pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso",&pBlob));
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr, &pVertexShader));

	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	GFX_THROW_INFO(pDevice->CreateInputLayout(ied, (UINT)std::size(ied),pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),&pInputLayout));


	// bind (vertex) layout
	pContext->IASetInputLayout(pInputLayout.Get());

 
	// bind render targt
	// moved
	
	
	// Set Primitive Topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set viewport
	D3D11_VIEWPORT vp[4];

	// ViewPort0
	vp[0].Width = 800;
	vp[0].Height = 600;
	vp[0].MinDepth = 0;
	vp[0].MaxDepth = 1;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;

	// ViewPort1
	vp[1].Width = 400;
	vp[1].Height = 300;
	vp[1].MinDepth = 0;
	vp[1].MaxDepth = 1;
	vp[1].TopLeftX = 400;
	vp[1].TopLeftY = 0;

	// ViewPort2
	vp[2].Width = 400;
	vp[2].Height = 300;
	vp[2].MinDepth = 0;
	vp[2].MaxDepth = 1;
	vp[2].TopLeftX = 0;
	vp[2].TopLeftY = 300;

	// ViewPort3
	vp[3].Width = 400;
	vp[3].Height = 300;
	vp[3].MinDepth = 0;
	vp[3].MaxDepth = 1;
	vp[3].TopLeftX = 400;
	vp[3].TopLeftY = 300;


	pContext->RSSetViewports(1u, &vp[i_vp]);



	// Draw
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices),0u, 0u));
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