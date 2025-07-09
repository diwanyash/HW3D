#include "Window.h"
#include <sstream>
#include "WindowsThrowMacros.h"
#include "imgui/backends/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;



Window::WindowClass::WindowClass() noexcept
	:
	hInstance( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(ClassName,GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return ClassName;
}
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInstance;
}

Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	
	if ( AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0 )
	{
		throw DEWND_LAST_EXCEPT();
	}
	

	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr)
	{
		throw DEWND_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// imit imgui win32 impl
	ImGui_ImplWin32_Init( hWnd );

	// point gfx
	pGfx = std::make_unique<Graphics>(hWnd, width, height);
}

Window::~Window()
{
	// destroy imgui
	ImGui_ImplWin32_Shutdown();
	
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title) const
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw DEWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessage() noexcept
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	
	}
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw DEWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

void Window::EnableCursor()
{
	CursorEnabled = true;
	ShowCursor();
}

void Window::DisableCursor()
{
	CursorEnabled = false;
	HideCursor();
}

void Window::HideCursor()
{
	while ( ::ShowCursor( FALSE ) >= 0 );
}

void Window::ShowCursor()
{
	while ( ::ShowCursor( TRUE ) < 0 );
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		// get pointer to struct
		const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		// get pointer to passed THIS in constructor of windows class
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));


		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// call handleMsg
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// imgui handle massage
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg , wParam, lParam))
	{
		return true;
	}
	const auto imio = ImGui::GetIO();

	switch (msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			/**************** Keypress Messages Start ****************/
		case WM_KILLFOCUS:
			kbd.ClearState();
			break;
			// check for system key
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if (imio.WantCaptureKeyboard)
			{
				break;
			}
			if( !(lParam & 0x40000000) || kbd.AutoReapeatIsEnabled() )
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
			break;
			// check for system key
		case WM_SYSKEYUP:
		case WM_KEYUP:
			if (imio.WantCaptureKeyboard)
			{
				break;
			}
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
			break;
		case WM_CHAR:
			if (imio.WantCaptureKeyboard)
			{
				break;
			}
			kbd.OnChar(static_cast<unsigned char>(wParam));
			break;
			/**************** Keypress Messages End ******************/


			/**************** Mouse Event Start **********************/
		case WM_MOUSEMOVE:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}

			const POINTS pt = MAKEPOINTS(lParam);
			if (pt.x > 0 && pt.x < width && pt.y > 0 && pt.y < height)
			{
				mouse.OnMouseMove(pt.x, pt.y);
				if ( !mouse.IsInWindow() )
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
			else
			{
				if (mouse.LeftIsPressed() || mouse.RightIsPressed())
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
		}
			break;
		case WM_RBUTTONDOWN:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
		}
			break;
		case WM_LBUTTONUP:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
		}
			break;
		case WM_RBUTTONUP:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
		}
			break;
		case WM_MOUSEWHEEL:
		{
			if (imio.WantCaptureMouse)
			{
				break;
			}
			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
		}
			break;
		/****************** Mouse Event End *******************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentyfied error code";
	}
	std::string errorstring = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorstring;
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{
}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Dead Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Dead Window Exception [No Graphics]";
}


