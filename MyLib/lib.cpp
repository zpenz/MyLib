# include "stdafx.h"
# include "lib.h"
# include <windows.h>

namespace LIB_WINDOW
{

 void ErrorMessage(const char * _error)
 {
	 MessageBox(NULL,_error,"error",MB_OK);
	 exit(0);
 }

 void BaseWindow::MoveCenterWindow()
 {
	 int tempWidth = SCREEN_WIDTH;
	 int tempHeight = SCREEN_HEIGHT;
	 int centerX = tempWidth / 2;
	 int centerY = tempHeight / 2;
	 centerX = centerX - GetWidth() / 2;
	 centerY = centerY - GetHeight() / 2;
	 POINT tempPoint = {centerX,centerY};
	 SetLeftTopPos(tempPoint);
 }

 BaseWindow::BaseWindow(int width, int height,
	 const string windowname, const string classname, DWORD WindowStyleEx, DWORD WindowStyle,POINT leftUpper)
	 :mWidth(width), mHeight(height),mClassname(classname), mWindowname(windowname), mWindowStyle(WindowStyle),
	 mWindowStyleEx(WindowStyleEx),mLeftTop(leftUpper)
 {
 }

 BaseWindow::BaseWindow():mWidth(1024),mHeight(768),mBaseHwnd(NULL),
		mClassname("LIB"),mWindowname("LIB"),mWindowStyle(WS_OVERLAPPEDWINDOW),
	mWindowStyleEx(WS_EX_APPWINDOW)
{

}

bool BaseWindow::ShowThisWindow()
{
	WNDCLASSEX wndcls;
	ZeroMemory(&wndcls,sizeof(wndcls));

	wndcls.cbClsExtra = 0;
	wndcls.cbSize     = sizeof(WNDCLASSEX);
	wndcls.cbWndExtra = 0;
	wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if(mInstance) 	wndcls.hInstance = mInstance;
	else wndcls.hInstance = NULL;
	
	if(!mCallBackFunc) 	wndcls.lpfnWndProc = WinProc;
	else wndcls.lpfnWndProc = mCallBackFunc;

	wndcls.lpszClassName = mClassname.c_str();
	wndcls.lpszMenuName = NULL;

	if (!mWindowStyle) 	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	else wndcls.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClassEx(&wndcls);

	InitBeforeCreate();
	MoveCenterWindow();

	mBaseHwnd = CreateWindowEx(mWindowStyleEx,mClassname.c_str(),mWindowname.c_str(),
		mWindowStyle,mLeftTop.x,mLeftTop.y,mWidth,mHeight,NULL,NULL,NULL,this);

	if(!mBaseHwnd)
	{
		ErrorMessage("创建窗口失败");
		return false;
	}

	AfterCreate();

	SetWindowLong(mBaseHwnd,GWL_USERDATA,(LONG)this);
	ShowWindow(mBaseHwnd,SW_SHOWNORMAL);
	UpdateWindow(mBaseHwnd);

	MessageLoop();
	return true;
}

void BaseWindow::MoveWindow() const
{
	if(mBaseHwnd)
	::MoveWindow(mBaseHwnd, mLeftTop.x, mLeftTop.y, mWidth, mHeight, true);
}

void BaseWindow::UpdateCache(bool topMost)
{
	::SetWindowPos(mBaseHwnd, topMost?HWND_TOPMOST:HWND_TOP, mLeftTop.x, mLeftTop.y,
		mWidth, mHeight, SWP_FRAMECHANGED);
}

void BaseWindow::UpdatePosition(WPARAM wParam, LPARAM lParam)
{
	mLeftTop = { LOWORD(lParam),HIWORD(lParam) };
}

bool BaseWindow::Show()
{
	auto proc = [](LPVOID lpParameter)-> DWORD WINAPI
	{
		BaseWindow * pWindow = reinterpret_cast<BaseWindow *>(lpParameter);
		// -------------- 
		pWindow->ShowThisWindow();
		return 0;
	};

	DWORD hThreadId;
	auto hThread = CreateThread(NULL, 0, proc, this, 0, &hThreadId); 
	if (!hThread) return false;
	auto error = GetLastError();

	CloseHandle(hThread);
	return true;
}

void BaseWindow::SetCallBackFunc(pCallBackFunc mFunc)
{
	mCallBackFunc = mFunc;
	if (mBaseHwnd) SetWindowLong(mBaseHwnd, GWL_WNDPROC, (LONG)mFunc);
}

void BaseWindow::SetInstance(HINSTANCE hInstance)
{
	mInstance = hInstance;
	if (mBaseHwnd) SetWindowLong(mBaseHwnd, GWL_HINSTANCE, (LONG)mInstance);
}

void BaseWindow::SetWidth(int Width)
{
	mWidth = Width;
	if (mBaseHwnd) MoveWindow();
}

void BaseWindow::SetHeight(int Height)
{
	mHeight = Height;
	if (mBaseHwnd) MoveWindow();
}

void BaseWindow::SetWindowName(const string windowname)
{
	mWindowname = windowname;
	if(mBaseHwnd)
	::SetWindowText(GetHwnd(), mWindowname.c_str());
}

void BaseWindow::AddWindowStyle(DWORD WindowStyle)
{
	mWindowStyle = mWindowStyle | WindowStyle;
	if (mBaseHwnd)
	{
		auto TempStyle = GetWindowLong(mBaseHwnd,GWL_STYLE);
		SetWindowLong(mBaseHwnd, GWL_STYLE, TempStyle|WindowStyle);
		UpdateCache(false);
	}
}

void BaseWindow::ReduceWindowStyle(DWORD WindowStyle)
{

	if (mWindowStyle & WindowStyle)
	{
		mWindowStyle = mWindowStyle & ~WindowStyle;
		if (mBaseHwnd)
		{
			auto TempStyle = GetWindowLong(mBaseHwnd, GWL_STYLE);
			SetWindowLong(mBaseHwnd, GWL_STYLE, TempStyle & ~WindowStyle);
			UpdateCache(false);
		}
	}
}

void BaseWindow::AddWindowStyleEx(DWORD WindowStyleEx)
{
	mWindowStyleEx = mWindowStyleEx | WindowStyleEx;
	if (mBaseHwnd)
	{
		auto TempStyle = GetWindowLong(mBaseHwnd, GWL_EXSTYLE);
		SetWindowLong(mBaseHwnd, GWL_EXSTYLE, TempStyle|WindowStyleEx);
		UpdateCache(false);
	}
}

void BaseWindow::ReduceWindowStyleEx(DWORD WindowStyleEx)
{
	if (mWindowStyleEx & WindowStyleEx)
	{
		mWindowStyleEx = mWindowStyleEx & ~WindowStyleEx;
		auto TempStyle = GetWindowLong(mBaseHwnd, GWL_STYLE);
		if (mBaseHwnd) SetWindowLong(mBaseHwnd, GWL_EXSTYLE, TempStyle&~WindowStyleEx);
		UpdateCache(false);
	}
}

void BaseWindow::SetLeftTopPos(POINT leftUpper)
{
	mLeftTop = leftUpper;  
	if(mBaseHwnd) MoveWindow();
}

void BaseWindow::SetWindowAlpha(int alpha)
{
	if (alpha < 0 || alpha >256) alpha = alpha | 256;

	if (!mBaseHwnd)
	{
		LOG_WARNING(Conver::Format("%s调用错误!",__FUNCTION__).c_str());
		return;
	}
	AddWindowStyleEx(WS_EX_LAYERED);
	::SetLayeredWindowAttributes(mBaseHwnd,RGB(0,255,0),alpha, LWA_ALPHA);
	UpdateCache(true);
}

int BaseWindow::GetWidth(void) const
{
	return mWidth;
}

int BaseWindow::GetHeight(void) const
{
	return mHeight;
}

string BaseWindow::GetWindowName(void) const
{
	return mWindowname;
}

HDC BaseWindow::GetHDC(void) const
{
	return mHdc;
}

HWND BaseWindow::GetHwnd(void) const
{
	return mBaseHwnd;
}

void BaseWindow::MessageLoop()
{
	MSG Message;
	PeekMessage(&Message,NULL,0,0,PM_NOREMOVE);
	static float fLastTime  = static_cast<float>(GetTickCount());
	while(Message.message!= WM_QUIT)
	{
		auto bHasMessage = PeekMessage(&Message,NULL,0,0,PM_REMOVE);
		if(bHasMessage)
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		else
		{
			float fIntervalTime = static_cast<float>(GetTickCount())-fLastTime;
			Update(fIntervalTime);
			fLastTime = fIntervalTime;
		}	
	}
}

//WM_NCCREATE posted before WM_CREATE
LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static HDC hDC;
	static BaseWindow * window;

	switch (uMsg)
	{
		case WM_CREATE:
			hDC = GetDC(hWnd);
			window->OnCreate();
			break;
		case WM_DESTROY:
			window->Destory();
			if(hDC) ReleaseDC(hWnd, hDC);
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			if(window==NULL) 
				ErrorMessage("window==NULL");
			window->OnDraw();
			break;
		case WM_NCCREATE:
			window = (BaseWindow*)(((CREATESTRUCT *)lParam)->lpCreateParams);
			break;
		case WM_NCPAINT:
			window->OnNcPaint(wParam);
			break;
		case WM_NCCALCSIZE:
			window->OnNcCalcSize(wParam,lParam);
			break;
		case WM_KEYUP:
			break;
		case WM_KEYDOWN:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_LBUTTONDOWN:
			window->OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			break;
		case WM_SIZING:
			window->ReDraw();
			break;
		case WM_MOVE:
			window->UpdatePosition(wParam,lParam);
			break;
		case WM_COMMAND:
			auto ControlId = LOWORD(wParam);
			switch(ControlId)
			{
			case 9999:
				LIB_CONTROL::Control ctest;
				ctest.AttachParent(window->GetHwnd());
				auto ret = ctest.CreateObject(300,100,100,50);
				break;
			}
			break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void BaseWindow::Update(float delta)
{
}

void BaseWindow::Destory()
{
	DestroyWindow(mBaseHwnd);
}

 BaseWindow::~BaseWindow()
{
}

 void BaseWindow::OnDraw()
 {
 }

 void BaseWindow::OnCreate()
 { 

 }

 void BaseWindow::OnNcPaint(WPARAM wParam)
 {	
 }

 void BaseWindow::InitBeforeCreate()
 {

 }

 void BaseWindow::ReDraw()
 {
	RECT rect;
	GetClientRect(mBaseHwnd,&rect);
	InvalidateRect(mBaseHwnd,&rect,true);
 }


 void BaseWindow::AfterCreate()
 {
	 //test for subwindow
	 auto hButton = CreateWindow(
		 "BUTTON", "嘿嘿", WS_VISIBLE | WS_CHILD, 0, 0, 200, 200, mBaseHwnd, (HMENU)9999, NULL, 0);
	 auto error = GetLastError();
 }

 void BaseWindow::OnNcCalcSize(WPARAM wParam,LPARAM lParam)
 {
 }

 void BaseWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {
 } 

 }