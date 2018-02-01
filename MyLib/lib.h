# pragma once
# include <stdio.h>
# include <cstdlib>
# include <windows.h>
# include "Control.h"
# include <string>
#include "Macros.h"

namespace LIB_WINDOW
{
	using namespace std;

class  BaseWindow
{
protected:
	int mWidth;
	int mHeight;
	bool mIsfullscreen;
	const string mClassname; 
	string mWindowname;

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
	void UpdatePosition(WPARAM wParam,LPARAM lParam);
public:
	BaseWindow(int width, int height,const string windowname, const string classname, 
		DWORD WindowStyleEx, DWORD WindowStyle, POINT leftUpper);
	BaseWindow();
	virtual ~BaseWindow();

	//InvalidRect
	void ReDraw();
	bool Show();

	void SetCallBackFunc(pCallBackFunc mFunc);
	void SetInstance(HINSTANCE hInstance);
	void SetWidth(int Width);
	void SetHeight(int Height);
	void SetWindowName(const string windowname);
	void AddWindowStyle(DWORD WindowStyle);
	void ReduceWindowStyle(DWORD WindowStyle);
	void AddWindowStyleEx(DWORD WindowStyleEx);
	void ReduceWindowStyleEx(DWORD WindowStyleEx);
	void SetLeftTopPos(POINT leftUpper);

	void SetWindowAlpha(int alpha); //窗口透明度 0~255 透明度

	int GetWidth(void)const;
	int GetHeight(void)const;
	string GetWindowName(void)const;
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
}

using namespace LIB_WINDOW;
