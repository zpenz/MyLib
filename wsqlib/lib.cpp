# include "stdafx.h"
# include "lib.h"
# include <windows.h>

 void Error_Box(const char * _error)
 {
	 MessageBox(NULL,_error,"error",MB_OK);
 }

 //计算坐标
 void My_Window::ChangeSize()
 {
	 //屏幕宽度和高度
	 int x = GetSystemMetrics(SM_CXSCREEN);
	 int y = GetSystemMetrics(SM_CYSCREEN);
	 //计算屏幕中心坐标
	 int center_x = x / 2;
	 int center_y = y / 2;
	 //计算窗口左上角坐标
	 this->mLeftTop.x = center_x - this->m_width  / 2;
	 this->mLeftTop.y = center_y - this->m_height / 2;
 }

My_Window::My_Window():m_width(1024),m_height(768),m_isfullscreen(false),hBaseWnd(NULL),
		m_classname("lib"),mwindowname("LIB"),m_lpvoid(this),mWindowStyle(WS_OVERLAPPEDWINDOW),
	mWindowStyleEx(WS_EX_APPWINDOW),mHInstance(GetModuleHandle(NULL)),m_hdc(NULL),m_background(NULL)
{
}

bool My_Window::Create()
{
	WNDCLASSEX wndcls;
	ZeroMemory(&wndcls,sizeof(wndcls));
	wndcls.cbClsExtra = 0;
	wndcls.cbSize     = sizeof(WNDCLASSEX);
	wndcls.cbWndExtra = 0;
	if(this->m_background!=NULL)
	wndcls.hbrBackground = this->m_background;
	else 
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hCursor    = LoadCursor(NULL,IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	if(mHInstance)
	wndcls.hInstance  = mHInstance;
	else
	wndcls.hInstance  = NULL;
	wndcls.lpfnWndProc = WinProc;
	wndcls.lpszClassName = m_classname;
	wndcls.lpszMenuName = NULL;
	wndcls.style        = CS_HREDRAW|CS_VREDRAW;
	RegisterClassEx(&wndcls);
	//Init
	this->Init();
	//调整窗口大小
	this->ChangeSize();
	hBaseWnd = CreateWindowEx(mWindowStyleEx,m_classname,mwindowname,mWindowStyle,this->mLeftTop.x,mLeftTop.y,m_width,m_height,NULL,NULL,NULL,this);
	if(!hBaseWnd)
	{
		Error_Box("创建窗口失败");
		return false;
	}
	//保存当前对象
	SetWindowLong(hBaseWnd,GWL_USERDATA,(LONG)this);
	ShowWindow(hBaseWnd,SW_SHOWNORMAL);
	UpdateWindow(hBaseWnd);
	//AfterCreate
	this->AfterCreate();
	MessageLoop();
	return true;
}

void My_Window::MessageLoop()
{
	int fmessage;
	MSG message;
	PeekMessage(&message,NULL,0,0,PM_NOREMOVE);
	static float last_time ;
	last_time = (float)GetTickCount();
	while(message.message!= WM_QUIT)
	{
		fmessage = PeekMessage(&message,NULL,0,0,PM_REMOVE);
		if(fmessage)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			float current_time = (float)GetTickCount()-last_time;
			this->OwnOperator(current_time);
			last_time = current_time;
		}	
	}
}

//============================
//WinProc
//WM_NCCREATE posted before by
//WM_CREATE
//============================
LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static HDC hDC;
	static My_Window * window;
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
			Error_Box("window==NULL");
		// Draw();
		window->OnDraw();
		break;
	case WM_NCCREATE:
		window = (My_Window*)(((CREATESTRUCT *)lParam)->lpCreateParams);
		break;
	case WM_NCPAINT:
		window->DrawSurface(wParam);
		break;
	case WM_NCCALCSIZE:
		window->CalSize(wParam,lParam);
		break;
	case WM_KEYUP:
		window->key.SetRelease(wParam);
		window->OnKeyUp();
		break;
	case WM_KEYDOWN:
		window->key.SetPress(wParam);
		window->OnKeyDown();
		break;
	case WM_MOUSEMOVE:
		window->SetMousePos(LOWORD(lParam),HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		window->key.SetLDown(true);
		window->OnLButtonDown(wParam, lParam);
		break;
	case WM_LBUTTONUP:
		window->key.SetLDown(false);
		break;
	case WM_SIZING:
		window->ReDraw();
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//================
//OwnOperator
//================
void My_Window::OwnOperator(float delta)
{
}

void My_Window::Destory()
{
	DestroyWindow(hBaseWnd);
}

//================
//~My_Window
//================
 My_Window::~My_Window()
{
}

 //===============
 //OnDraw 
 //===============
 void My_Window::OnDraw()
 {
 }

 //===============
 //OnCreate
 //===============
 void My_Window::OnCreate()
 { 
 }

 void My_Window::DrawSurface(WPARAM wParam)
 {	
 }

 //============
 //初始化 在CreateWindow之前
 //============
 void My_Window::Init()
 {

 }
 //===========
 //void OwnMessageDo();
 //===========
 void My_Window::OwnMessageDo()
 {
 }

//======================
//press function
//void OnKeyDown();
//void OnKeyUp();
//void OnMouseMove();  
//======================
 void My_Window::OnKeyDown()
 {
 }

 void My_Window::OnKeyUp()
 {
 }

 void My_Window::OnMouseMove()
 {
 }

//================
//void ReDraw
//强制窗口重绘
//================
 void My_Window::ReDraw()
 {
	RECT rect;
	GetClientRect(hBaseWnd,&rect);
	InvalidateRect(hBaseWnd,&rect,true);
 }

 //===================
 //void AfterCreate()
 //after create window.
 //===================
 void My_Window::AfterCreate()
 {
 }

//======================
// 响应 WM_NCCALCSIZE
//======================
 void My_Window::CalSize(WPARAM wParam,LPARAM lParam)
 {
 }

 void My_Window::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {
 } 