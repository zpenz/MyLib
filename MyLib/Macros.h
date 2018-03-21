#pragma once
#include <windows.h>
#include <stdarg.h>
#include <string>
#include <map>
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
#define SAFE_RELEASE(p) if(p){p->Release(); p = nullptr;}
#define SAFE_DELETE(p)  if(p!=nullptr){delete p; p = nullptr;} 

#define MAX_BUF_LENGTH 4096
#define NoWarningBool(boolvalue) boolvalue==0?false:true;

//type-cast
#define STCAST(type,des) static_cast<type>(des)
#define COCAST(type,des) const_cast<type>(des)
#define RPCAST(type,des) reinterpret_cast<type>(des)
#define DYCAST(type,des) dynamic_cast<type>(des)

namespace Macros2D
{

//对齐方式
#define ALIGN_TEXT_LEFT			DWRITE_TEXT_ALIGNMENT_LEADING
#define ALIGN_TEXT_RIGHT		DWRITE_TEXT_ALIGNMENT_TRAILING
#define ALIGN_TEXT_CENTER		DWRITE_TEXT_ALIGNMENT_CENTER
  
#define ALIGN_PARAGRAPH_TOP		DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define ALIGN_PARAGRAPH_BOTTOM  DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define ALIGN_PARAGRAPH_CENTER  DWRITE_PARAGRAPH_ALIGNMENT_CENTER


#define ALIGN_TEXT_TYPE		     DWRITE_TEXT_ALIGNMENT
#define ALIGN_PARAGRAPH_TYPE     DWRITE_PARAGRAPH_ALIGNMENT

#define COLOREX(colorRef) D2D1::ColorF(GetRValue(colorRef)/255.0f,GetGValue(colorRef)/255.0f,GetBValue(colorRef)/255.0f,1.0f)
//#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define SetR(colorRef,r) RGB(r,GetGValue(colorRef),GetBValue(colorRef))
#define SetG(colorRef,g) RGB(GetRValue(colorRef),g,GetBValue(colorRef))
#define SetB(colorRef,b) RGB(GetRValue(colorRef),GetGValue(colorRef),b)
}

namespace MyMessage
{
#define SHOULD_DO_NOTHING	   0
#define SHOULD_CLOSE_WINDOW    678
#define SHOULD_MINI_WINDOW     679
#define SHOULD_MAX_WINDOW      680
#define SHOULD_RESTORE_WINDOW  681
}

namespace LIB_CONTROL
{
#define CONTROL_TYPE_NONE			0
#define CONTROL_TYPE_TITLEBAR		1
#define CONTROL_TYPE_BUTTON			2
#define CONTROL_TYPE_MAXI_BUTTON    3
#define CONTROL_TYPE_MINI_BUTTON    4
#define CONTROL_TYPE_CLOSE_BUTTON   5
#define CONTROL_TYPE_RESTORE_BUTTON 6
#define CONTROL_TYPE_DEFAULT_BUTTON 7
}

namespace LIB_WINDOW
{
	namespace MOUSE
	{
#define MOUSE_STATE_IDLE              0  
#define MOUSE_STATE_LEFTBUTTONDOWN    1	
#define MOUSE_STATE_RIGHTBUTTONDOWN   2
#define MOUSE_STATE_LEFTBUTTONUP      3
#define MOUSE_STATE_RIGHTBUTTONUP     4
#define MOUSE_STATE_MOUSEMOVE         5
#define MOUSE_STATE_STROLLWHEEL       6
#define MOUSE_STATE_UP                7
#define MOUSE_STATE_DOWN              8
	}
}

namespace ARROW
{
	class ArrowShape
	{
	public:
		static HCURSOR SHAPE_ARROW;
		static HCURSOR SHAPE_I;
	};
}

typedef struct 
{
	BYTE r, g, b;

	COLORREF getRGB() { return RGB(r,g,b); }
}ColorStruct;

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
		OutputDebugStringA("[ERROR]xxxxxxxxxxxxxxxxxx-------------   ");\
		OutputDebugStringA(error_message);\
		OutputDebugStringA("\n");\
		return returnValue;}

#define IS_CONTINUE_ERROR(condition,error_message)\
		if(condition)\
		{\
		OutputDebugStringA("[ERROR]xxxxxxxxxxxxxxxxxx-------------    ");\
		OutputDebugStringA(error_message);\
		OutputDebugStringA("\n");\
		continue;}

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
#define nullRect MyRect{0,0,0,0}
	using namespace std;

	string Format(char * format,...);

	POINT  CenterPoint(RECT rc, int offsetX = 0,int offsetY = 0);

	POINT LeftTopPoint(RECT rc);

	POINT TopCenterPoint(RECT rc);

	POINT BottomCenterPoint(RECT rc);

	POINT LeftCenterPoint(RECT rc);

	POINT RightCenterPoint(RECT rc);

	POINT RightBottomPoint(RECT rc);

	POINT LeftBottomPoint(RECT rc);

	POINT RightTopPoint(RECT rc);

	RECT ClientToScreenRc(HWND hWnd, RECT & rc);

	RECT ScreenToClientRc(HWND hWnd, RECT & rc);

	///宽字节和ASCI互相转换
	char * WCharToAChar(wchar_t * Wchar,UINT codePage = CP_UTF8);

	wchar_t * ACharToWChar(char * Achar,UINT codePage = CP_UTF8);

	int utoi(wchar_t * Wchar);

	///<d2dConver>some d2d conver</d2dConver>
	D2D1_POINT_2F & PointToD2DPointF(POINT & pt);

	D2D1_RECT_F & RectToD2DRectF(RECT & rc);

	bool PointInRect(int x, int y,RECT rc);

	bool RectInRect(RECT smallRect, RECT bigRect);

	bool ZeroRect(RECT targetRect); //零矩形...

	bool VaildRect(RECT targetRect); //矩形是否合理

	RECT ClipRectBoard(RECT srcRect,LONG dx,LONG dy);

	RECT SubRect(RECT srcRect,RECT desRect); // sub Rect

	RECT ClipRectBoardEx(RECT srcRect, LONG lLeft, LONG lTop,LONG lRight,LONG lBottom);

	RECT RectOffSet(RECT srcRect, LONG lLeft, LONG lTop, LONG lRight, LONG lBottom); // default + 

	RECT GetMaxSizeRect(); //获取最大化区域

	class MyRect 
	{
	private:
		RECT mRect;

		int mLeft, mRight, mTop, mBottom;
	public:
		MyRect(INT left, INT top, INT right, INT bottom);

		MyRect(RECT desRect);

		MyRect(POINT pointLeftTop, POINT pointRightBottom);

		RECT Rect() const;

		operator RECT &();

		operator D2D1_RECT_F&();

	};

	class Point 
	{
	public:
		int x, y;

		Point operator= (POINT & pt);

		operator POINT();

		Point operator-(POINT pt);

		Point operator+(POINT pt);

		Point(int _x,int _y);

		Point(POINT pt) ;

		Point();
	
	};

	wstring GetDirName(wstring path);
}



typedef LRESULT(_stdcall *pCallBackFunc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
