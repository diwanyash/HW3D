#pragma once
#include "DeadWin.h"
#include "DeadException.h"
#include "KeyBoard.h"

class Window{
public:
	class Exception : public Deadexception
	{
	public:
		Exception(int line,const char* file,HRESULT hr)noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode()const noexcept;
		std::string GetErrorString()const noexcept;
	private:
		HRESULT hr;
	};
private:
	class WindowClass{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass( const WindowClass& ) = delete;
		WindowClass& operator=( const WindowClass& ) = delete;
		static constexpr const char* ClassName = "Dead Direct 3D Window";
		static WindowClass wndClass;
		HINSTANCE hInstance;
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	KeyBoard kbd;
private:
	int width;
	int height;
	HWND hWnd;
	
};


#define DEWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
#define DEWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__,GetLastError());