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

void ErrorMessage(const char * _error);

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

#define IS_ERROR_EXIT(condition,sErrorMessage)\
		if(condition)\
		{\
			ErrorMessage(sErrorMessage);\
		}
		
namespace Conver
{

#define RECTWIDTH(rc) rc.right - rc.left
#define RECTHEIGHT(rc) rc.bottom - rc.top

	using namespace std;
	string Format(char * format,...);
	POINT  CenterPoint(RECT rc, float offsetX = 0.00,float offsetY = 0.00);
	POINT LeftTopPoint(RECT rc);
	///<ÏÈÁô×Å¿Ó>
	char * WCharToAChar(wchar_t * Wchar);
	wchar_t * ACharToWChar(wchar_t * Wchar);
}

typedef LRESULT(_stdcall *pCallBackFunc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);