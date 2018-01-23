# pragma once
# include <stdio.h>
# include <cstdlib>
# include <windows.h>

# ifdef INCLUDE_MY_DX9
# include "libD3D.h"
# endif

#define SCREEN_WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
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
		OutputDebugStringA("[ERROR]------------------------------------");\
		OutputDebugStringA(error_message);\
		OutputDebugStringA("\n");\
		return returnValue;\
		}
		
 class  BaseWindow
{
private:
	typedef struct { int x; int y; }Point;

	int mWidth;
	int mHeight;
	bool mIsfullscreen;
	const char * mClassname; 
	const char * mWindowname;

	DWORD mWindowStyleEx;
	DWORD mWindowStyle;
	HINSTANCE mHInstance;
	HBRUSH mBackground;

	void * mLpvoid;
	Point mLeftTop;

	HDC  mHdc;
	HWND mHBaseWnd; 

	Point mMousePos;

	void MoveCenterWindow();

public:

	//InvalidRect
	void ReDraw();

	BaseWindow();

	virtual ~BaseWindow();

	BaseWindow(int width, int height, bool isfullscreen, const char * windowname, const char * classname, DWORD WindowStyleEx, DWORD WindowStyle, void * lpvoid, Point leftUpper)
		:mWidth(width), mHeight(height), mIsfullscreen(isfullscreen), mWindowname(windowname), mClassname(classname),
		mWindowStyle(WindowStyle), mWindowStyleEx(WindowStyleEx) {}

	bool Create(void);

	void SetInstance(HINSTANCE hInstance) {mHInstance = hInstance;}
	void SetWidth(int Width) {if(Width>0|| Width<SCREEN_WIDTH){mWidth = Width;}else{mWidth = 1024;}}
	void SetHeight(int Height) {if(Height>0|| Height<SCREEN_HEIGHT){mHeight = Height;}else{mHeight = 768;}}
	void SetWindowName(const char * windowname){mWindowname = windowname;}
	void SetClassName(const char * classname){mClassname = classname;}
	void SetWindowStyle(DWORD WindowStyle){mWindowStyle=WindowStyle;}
	void SetWindowStyleEx(DWORD WindowStyleEx){mWindowStyleEx = WindowStyleEx;}
	void SetWindowPos(Point leftUpper){mLeftTop = leftUpper;}
	void SetUserData(LPVOID lpvoid){mLpvoid = lpvoid;}
	void SetFullScrren(bool isfullscreen){mIsfullscreen = isfullscreen;}
	void SetMousePos(int px,int py) {mMousePos.x = px; mMousePos.y = py;}
	void SetBrush(const HBRUSH & bs) {mBackground = bs;}

	int GetWidth(void) {return mWidth;}
	int GetHeight(void) {return mHeight;}
	const char * GetWindowName(void){return mWindowname;}

	Point GetMousePos(void){return mMousePos;}
	
	bool SetHDC(HDC hDC){mHdc = hDC; return hDC?true:false;}
	HDC  GetHDC(void){return mHdc;}
	HWND GetHwnd(void){return mHBaseWnd;}
	
	virtual void Destory();
	virtual void MessageLoop();
	virtual void Update(float delta);
	virtual void InitBeforeCreate();
	virtual void AfterCreate();

	//default draw operate
	virtual void OnDraw();
	virtual void OwnMessageDo();
	virtual void OnCreate();
	virtual void DrawSurface(WPARAM wParam);
	virtual void CalSize(WPARAM wParam,LPARAM lParam);
	virtual void OnLButtonDown(WPARAM wParam,LPARAM lParam);

	//pree class
	virtual void OnKeyDown();
	virtual void OnKeyUp();
	virtual void OnMouseMove();

	operator HDC()  {return mHdc;}
	operator HWND() {return mHBaseWnd;}

	friend 	
		LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

