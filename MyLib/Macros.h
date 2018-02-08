#pragma once
#include <windows.h>
#include <stdarg.h>
#include <string>
#include <d2d1.h>
# ifdef INCLUDE_MY_DX9
# include "libD3D.h"
# endif

# ifdef INCLUDE_MY_DX11
# include "libd11.h"
# endif

#define SCREEN_WIDTH  GetDeviceCaps(GetDC(NULL),HORZRES)
#define SCREEN_HEIGHT GetDeviceCaps(GetDC(NULL),VERTRES)
#define MAX_KEYS 256
#define SAFE_RELEASE(p) if(p){p->Release(); p = NULL;}
#define SAFE_DELETE(p)  if(p){delete p; p = NULL;} 

namespace Macros2D
{
	//对齐方式
#define ALIGN_DEFAULT    0
#define ALIGN_LEFT       1
#define ALIGN_RIGHT      2
#define ALIGN_TOP        3
#define ALIGN_BOTTOM     4
#define ALIGN_CENTER_V   5
#define ALIGN_CENTER_H   6

#define COLOREX(colorRef) D2D1::ColorF(GetRValue(colorRef)/255.0f,GetGValue(colorRef)/255.0f,GetBValue(colorRef)/255.0f,1.0f)
}

namespace MyMessage
{
#define SHOULD_CLOSE_WINDOW 678
#define SHOULD_MINI_WINDOW 679
#define SHOULD_MAX_WINDOW 680
#define SHOULD_DO_NOTHING 0
}

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

	POINT  CenterPoint(RECT rc, int offsetX = 0,int offsetY = 0);

	POINT LeftTopPoint(RECT rc);

	POINT RightBottomPoint(RECT rc);

	POINT LeftBottomPoint(RECT rc);

	POINT RightTopPoint(RECT rc);

	RECT ClientToScreenRc(HWND hWnd, RECT & rc);

	RECT ScreenToClientRc(HWND hWnd, RECT & rc);

	///宽字节和ASCI互相转换
	char * WCharToAChar(wchar_t * Wchar);

	wchar_t * ACharToWChar(char * Achar);

	///<d2dConver>some d2d conver</d2dConver>
	D2D1_POINT_2F & PointToD2DPointF(POINT & pt);

	D2D1_RECT_F & RectToD2DRectF(RECT & rc);

	bool PointInRect(int x, int y,RECT rc);

	bool RectInRect(RECT smallRect, RECT bigRect);

	RECT ClipRectBoard(RECT srcRect,LONG dx,LONG dy);

	class MyRect
	{
	private:
		RECT mRect;

		int mLeft, mRight, mTop, mBottom;
	public:
		MyRect(INT left, INT top, INT right, INT bottom);

		MyRect(RECT desRect);

		RECT Rect() const;

		operator RECT &();

		operator D2D1_RECT_F&();
	};
}

typedef LRESULT(_stdcall *pCallBackFunc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);