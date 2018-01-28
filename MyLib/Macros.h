#pragma once
#include <windows.h>
#include <stdarg.h>
#include <string>
# ifdef INCLUDE_MY_DX9
# include "libD3D.h"
# endif

# ifdef INCLUDE_MY_DX11
# include "libd11.h"
# endif

#define SCREEN_WIDTH  GetDeviceCaps(GetDC(NULL),HORZRES)
#define SCREEN_HEIGHT GetDeviceCaps(GetDC(NULL),VERTRES)
#define MAX_KEYS 256
# define SAFE_RELEASE(p) if(p){p->Release(); p=NULL;}

extern void ErrorMessage(const char * _error);

#define SINGLE_INSTANCE(classname) \
		private: \
			classname(){} \
		public:\
		inline static classname & getInstance(){\
			static classname  cInstance;\
			return cInstance;}

#define IS_RETURN_ERROR(condition,returnValue,error_message)\
		if(condition)\
		{\
		OutputDebugStringA("[ERROR]xxxxxxxxxxxxxxxxxx----");\
		OutputDebugStringA(error_message);\
		OutputDebugStringA("\n");\
		return returnValue;}

#define LOG_WARNING(format,...)\
		OutputDebugStringA(Conver::Format("[WARN]:%s\n",format).c_str());


#define IS_RETURN_FUNC(condition,returnValue,FUNC)\
		if(condition)\
		{\
		FUNC();\
		return returnValue;}

namespace Conver
{
	using namespace std;
	string Format(char * format,...);
}

typedef LRESULT(_stdcall *pCallBackFunc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);