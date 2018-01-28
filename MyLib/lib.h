# pragma once
# include <stdio.h>
# include <cstdlib>
# include <windows.h>
#include "Macros.h"

class  BaseWindow
{
protected:
	typedef struct { int x; int y; }Point;

	int mWidth;
	int mHeight;
	bool mIsfullscreen;
	const char * mClassname; 
	const char * mWindowname;

	DWORD mWindowStyleEx;
	DWORD mWindowStyle;
	HINSTANCE mInstance;

	POINT mLeftTop;

	HDC  mHdc;
	HWND mBaseHwnd; 

	pCallBackFunc mCallBackFunc;

private:
	void MoveCenterWindow();
	bool ShowThisWindow(void); //show this window
	void MoveWindow()const;
	void UpdateCache(bool topMost);
public:
	//InvalidRect
	void ReDraw();

	BaseWindow();

	virtual ~BaseWindow();

	BaseWindow(int width, int height, bool isfullscreen, const char * windowname, const char * classname, DWORD WindowStyleEx, DWORD WindowStyle, void * lpvoid, Point leftUpper)
		:mWidth(width), mHeight(height), mIsfullscreen(isfullscreen), mWindowname(windowname), mClassname(classname),
		mWindowStyle(WindowStyle), mWindowStyleEx(WindowStyleEx),mCallBackFunc(NULL){}

	bool Show();

	void SetCallBackFunc(pCallBackFunc mFunc);
	void SetInstance(HINSTANCE hInstance);
	void SetWidth(int Width);
	void SetHeight(int Height);
	void SetWindowName(const char * windowname);
	void AddWindowStyle(DWORD WindowStyle);
	void ReduceWindowStyle(DWORD WindowStyle);
	void AddWindowStyleEx(DWORD WindowStyleEx);
	void ReduceWindowStyleEx(DWORD WindowStyleEx);
	void SetLeftTopPos(POINT leftUpper);

	void SetWindowAlpha(int alpha); //窗口透明度 0~255 透明度

	int GetWidth(void)const;
	int GetHeight(void)const;
	const char * GetWindowName(void)const;
	HDC  GetHDC(void)const;
	HWND GetHwnd(void)const;
	
	virtual void Destory();
	virtual void MessageLoop();
	virtual void Update(float delta);
	virtual void InitBeforeCreate();
	virtual void AfterCreate();

	virtual void OnDraw();	//default draw operate
	virtual void OnCreate();
	virtual void OnNcPaint(WPARAM wParam);
	virtual void OnNcCalcSize(WPARAM wParam,LPARAM lParam);
	virtual void OnLButtonDown(WPARAM wParam,LPARAM lParam);

	operator HDC()  {return mHdc;}
	operator HWND() {return mBaseHwnd;}

	friend
	LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

