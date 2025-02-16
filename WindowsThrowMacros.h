#pragma once


// error exception helper macro
#define DEWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define DEWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define DEWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )