# include "stdafx.h"
# include "lib.h"
# include <windows.h>

namespace LIB_WINDOW
{
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

	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	
	RegisterClassEx(&wndcls);

	InitBeforeCreate();
	MoveCenterWindow();

	//Style
	mWindowStyleEx = WS_EX_LEFT | WS_EX_LTRREADING| WS_EX_RIGHTSCROLLBAR|WS_EX_WINDOWEDGE;

	mWindowStyle =  WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_SYSMENU | WS_THICKFRAME| WS_OVERLAPPED | WS_MINIMIZEBOX |WS_MAXIMIZEBOX ;

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
	auto proc = [](LPVOID lpParameter)-> DWORD 
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
	if (alpha < 0 || alpha >=256) alpha = alpha | 256;

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
			
			//re draw
			//DrawManager.Clear(MyColor::ColorF(45.0 / 256, 45.0 / 256, 48.0 / 256));
			ControlListener.Draw();

			RECT windowRect;
			GetWindowRect(mBaseHwnd, &windowRect);
			Conver::ScreenToClientRc(mBaseHwnd, windowRect);
			DrawManager.DrawRectangle(windowRect, MyColor::Blue, false);
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
			window->OnDraw();
			break;
		case WM_NCCREATE:
			window = (BaseWindow*)(((CREATESTRUCT *)lParam)->lpCreateParams);
			break;
		case WM_NCPAINT:
			window->OnNcPaint((HRGN)wParam);
			return 0;
			break;
		case WM_NCCALCSIZE:
			window->OnNcCalcSize(wParam,lParam);
			if ((BOOL)wParam) return 0;
			break;
		case WM_NCHITTEST:
			return window->OnHitTest(lParam);
		case WM_NCACTIVATE:
			return(window->OnNcActive(wParam,lParam));
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
			break;
		case WM_MOVE:
			window->UpdatePosition(wParam,lParam);
			break;
		case WM_COMMAND:
			auto ControlId = LOWORD(wParam);
			switch(ControlId)
			{
			case 9999:
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



 void BaseWindow::OnCreate()
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
	 //初始化D2D1.0
	 auto ret = DrawManager.InitManager();
	 IS_RETURN_ERROR(!ret, , "初始化D2D错误!");

	 ret = DrawManager.SetRenderTarget(mBaseHwnd);
	 IS_RETURN_ERROR(!ret, , "设置RenderTarget失败!");

	 //窗口区域转换 
	 auto Rgn = CreateRectRgn(0,0,mWidth,mHeight);
	 SetWindowRgn(mBaseHwnd, Rgn,true);
	 DeleteObject(Rgn);

	 //TitleBar
	 TitleBar * pBar = new TitleBar("pSong's Window",NULL);
	 pBar->AdjustRect(Conver::MyRect(0,0,mWidth,35));
	 pBar->SetBackColor(RGB(65,65,68));
	 pBar->SetHoverBackColor(RGB(116, 116, 119));
	 ControlListener.attach(pBar);
 }

 void BaseWindow::OnDraw()
 {

 }

 void BaseWindow::OnNcPaint(HRGN rgn)
 {
	 auto hdc = GetWindowDC(mBaseHwnd);
	 // Paint into this DC
	 if (hdc)
	 {
		 RECT windowRect;
		 GetWindowRect(mBaseHwnd, &windowRect);
		 TextOut(hdc, 400, 10, TEXT("abcd"), 4);
		 ReleaseDC(mBaseHwnd, hdc);
	 }
 }

 void BaseWindow::OnNcCalcSize(WPARAM wParam,LPARAM lParam)
 {
	 if (wParam == FALSE) return;
	 auto ncp = reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
	 RECT rc1 = ncp->rgrc[0];
	 RECT rc2 = ncp->rgrc[1];
	 RECT rc3 = ncp->rgrc[2];

 }

 bool BaseWindow::OnNcActive(WPARAM wParam, LPARAM lParam)
 {
	 return true;
 }

 UINT BaseWindow::OnHitTest(LPARAM lParam)
 {
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y};
	 ScreenToClient(mBaseHwnd,&pt);
	 return ControlListener.HitTest(pt);
 }

 void BaseWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {

 }

 }