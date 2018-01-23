# include "stdafx.h"
# include "lib.h"
# include <windows.h>

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
	 SetWidth(centerX - GetWidth() / 2);
	 SetHeight(centerY - GetHeight() / 2);
 }

BaseWindow::BaseWindow():mWidth(1024),mHeight(768),mIsfullscreen(false),mHBaseWnd(NULL),
		mClassname("lib"),mWindowname("LIB"),mLpvoid(this),mWindowStyle(WS_OVERLAPPEDWINDOW),
	mWindowStyleEx(WS_EX_APPWINDOW),mHInstance(GetModuleHandle(NULL)),mHdc(NULL),mBackground(NULL)
{
}

bool BaseWindow::Create()
{
	WNDCLASSEX wndcls;
	ZeroMemory(&wndcls,sizeof(wndcls));
	wndcls.cbClsExtra = 0;
	wndcls.cbSize     = sizeof(WNDCLASSEX);
	wndcls.cbWndExtra = 0;
	if(this->mBackground!=NULL)
	wndcls.hbrBackground = this->mBackground;
	else 
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hCursor    = LoadCursor(NULL,IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	if(mHInstance)
	wndcls.hInstance  = mHInstance;
	else
	wndcls.hInstance  = NULL;
	wndcls.lpfnWndProc = WinProc;
	wndcls.lpszClassName = mClassname;
	wndcls.lpszMenuName = NULL;
	wndcls.style        = CS_HREDRAW|CS_VREDRAW;
	RegisterClassEx(&wndcls);

	InitBeforeCreate();

	MoveCenterWindow();
	mHBaseWnd = CreateWindowEx(mWindowStyleEx,mClassname,mWindowname,
		mWindowStyle,mLeftTop.x,mLeftTop.y,mWidth,mHeight,NULL,NULL,NULL,this);
	if(!mHBaseWnd)
	{
		ErrorMessage("��������ʧ��");
		return false;
	}

	SetWindowLong(mHBaseWnd,GWL_USERDATA,(LONG)this);
	ShowWindow(mHBaseWnd,SW_SHOWNORMAL);
	UpdateWindow(mHBaseWnd);
	AfterCreate();
	MessageLoop();
	return true;
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
		window->SetHDC(hDC);
		window->OnCreate();
		break;
	case WM_DESTROY:
		window->Destory();
		window = NULL;
		ReleaseDC(hWnd,hDC);
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		//window = (My_Window *)GetWindowLong(hWnd,GWL_USERDATA);
		if(window==NULL) 
			ErrorMessage("window==NULL");
		window->OnDraw();
		break;
	case WM_NCCREATE:
		window = (BaseWindow*)(((CREATESTRUCT *)lParam)->lpCreateParams);
		break;
	case WM_NCPAINT:
		window->DrawSurface(wParam);
		break;
	case WM_NCCALCSIZE:
		window->CalSize(wParam,lParam);
		break;
	case WM_KEYUP:
		window->OnKeyUp();
		break;
	case WM_KEYDOWN:
		window->OnKeyDown();
		break;
	case WM_MOUSEMOVE:
		window->SetMousePos(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		window->OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		break;
	case WM_SIZING:
		window->ReDraw();
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void BaseWindow::Update(float delta)
{
}

void BaseWindow::Destory()
{
	DestroyWindow(mHBaseWnd);
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

 void BaseWindow::DrawSurface(WPARAM wParam)
 {	
 }

 void BaseWindow::InitBeforeCreate()
 {

 }

 void BaseWindow::OwnMessageDo()
 {
 }

 void BaseWindow::OnKeyDown()
 {
 }

 void BaseWindow::OnKeyUp()
 {
 }

 void BaseWindow::OnMouseMove()
 {
 }


 void BaseWindow::ReDraw()
 {
	RECT rect;
	GetClientRect(mHBaseWnd,&rect);
	InvalidateRect(mHBaseWnd,&rect,true);
 }


 void BaseWindow::AfterCreate()
 {
 }

 void BaseWindow::CalSize(WPARAM wParam,LPARAM lParam)
 {
 }

 void BaseWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {
 } 