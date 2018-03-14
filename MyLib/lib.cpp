# include "stdafx.h"
# include "lib.h"

namespace LIB_WINDOW
{
	RECT BaseWindow::Rect() const
	{
		if (!mBaseHwnd) return Conver::nullRect;
		RECT currentRect;
		GetWindowRect(mBaseHwnd,&currentRect);
		return currentRect;
	}

	bool BaseWindow::SetRect(RECT desRect)
	{
		if (!Conver::VaildRect(desRect)) return false;
		if (!mBaseHwnd) return false;
 		mWindowRect = desRect;
		mLeftTop = {desRect.left,desRect.top};
		mWidth = RECTWIDTH(desRect);
		mHeight = RECTHEIGHT(desRect);
		UpdateSize();
		return true;
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
	 const wstring windowname, const wstring classname, DWORD WindowStyleEx, DWORD WindowStyle,POINT leftUpper)
	 :mWidth(width), mHeight(height),mClassname(classname), mWindowname(windowname), mWindowStyle(WindowStyle),
	 mWindowStyleEx(WindowStyleEx),mLeftTop(leftUpper)
 {
 }

 BaseWindow::BaseWindow():mWidth(1024),mHeight(768),mBaseHwnd(NULL),
		mClassname(L"LIB"),mWindowname(L"LIB"),mWindowStyle(WS_OVERLAPPEDWINDOW),
	mWindowStyleEx(WS_EX_APPWINDOW)
{

}

bool BaseWindow::ShowThisWindow()
{
	WNDCLASSEXW wndcls;
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
	
	RegisterClassExW(&wndcls);

	InitBeforeCreate();
	MoveCenterWindow();

	//Style
	mWindowStyleEx = WS_EX_LEFT | WS_EX_LTRREADING| WS_EX_RIGHTSCROLLBAR|WS_EX_WINDOWEDGE;

	mWindowStyle =  WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_SYSMENU | WS_THICKFRAME| WS_OVERLAPPED | WS_MINIMIZEBOX |WS_MAXIMIZEBOX ;

	mBaseHwnd = CreateWindowExW(mWindowStyleEx,mClassname.c_str(),mWindowname.c_str(),
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

void BaseWindow::UpdateSize() const
{
	if(mBaseHwnd)
	::MoveWindow(mBaseHwnd, mLeftTop.x, mLeftTop.y, mWidth, mHeight, false);
}

void BaseWindow::UpdateCache(bool topMost)
{
	::SetWindowPos(mBaseHwnd, topMost?HWND_TOPMOST:HWND_TOP, mLeftTop.x, mLeftTop.y,
		mWidth, mHeight, SWP_FRAMECHANGED);
}

void BaseWindow::UpdatePosition(WPARAM wParam, LPARAM lParam)
{
	mLeftTop.x = (int)(short)LOWORD(lParam);
	mLeftTop.y = (int)(short)HIWORD(lParam);
}

bool BaseWindow::Show()
{
	auto proc = [](LPVOID lpParameter)-> DWORD 
	{
		BaseWindow * pWindow = reinterpret_cast<BaseWindow *>(lpParameter);
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
	if (mBaseHwnd) UpdateSize();
}

void BaseWindow::SetHeight(int Height)
{
	mHeight = Height;
	if (mBaseHwnd) UpdateSize();
}

void BaseWindow::SetWindowName(const wstring windowname)
{
	mWindowname = windowname;
	if(mBaseHwnd) ::SetWindowTextW(GetHwnd(), mWindowname.c_str());
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
	mLeftTop = { leftUpper.x,leftUpper.y};
	if(mBaseHwnd) UpdateSize();
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

wstring BaseWindow::GetWindowName(void) const
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
	PeekMessageW(&Message,NULL,0,0,PM_NOREMOVE);
	static float fLastTime  = static_cast<float>(GetTickCount());
	while(Message.message!= WM_QUIT)
	{
		auto bHasMessage = PeekMessageW(&Message,NULL,0,0,PM_REMOVE);
		if(bHasMessage)
		{
			TranslateMessage(&Message);
			DispatchMessageW(&Message);
		}
		else
		{
			float fIntervalTime = static_cast<float>(GetTickCount())-fLastTime;
			Update(fIntervalTime);
			fLastTime = fIntervalTime;
		}	
	}
}

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
			return window->OnNcActive(wParam,lParam);
			break;
		case WM_ERASEBKGND: 
			return 1; // 不擦除背景
		case WM_KEYUP:
			break;
		case WM_KEYDOWN:
			break;
		case WM_CHAR:
		case WM_UNICHAR:
		case WM_IME_CHAR:
			return window->OnUnicodeChar(wParam,lParam);
		case WM_MOUSEMOVE:
			window->OnMouseMove(wParam,lParam);
			break;
		case WM_NCLBUTTONDBLCLK:
			return 0;
		case WM_LBUTTONDOWN:
			window->OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			window->OnLButtonUp(wParam, lParam);
			break;
		case WM_SIZING:
			window->OnSizing(wParam, lParam);
			break;
		case WM_SIZE:
			return window->OnSize(wParam,lParam);
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

void BaseWindow::Destory()
{
	DestroyCaret(); // 销毁插入符
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

 void BaseWindow::ReDraw(bool bReDraw)
 {
	SendMessage(mBaseHwnd, WM_PAINT, NULL, NULL);
 }

 void BaseWindow::AfterCreate()
 {
	 //初始化D2D1.0
	 auto ret = DrawManager.InitManager();  
	 IS_RETURN_ERROR(!ret, , "初始化D2D错误!");
	 ret = DrawManager.SetRenderTarget(mBaseHwnd);
	 IS_RETURN_ERROR(!ret, , "设置RenderTarget失败!");
	 DrawManager.UseTempRenderTarget();

	 CaretManager.attrach(mBaseHwnd);
	 CaretManager.InitCaret();

	 //窗口区域转换 
	 //auto Rgn = CreateRectRgn(0,0,mWidth,mHeight);
	 //SetWindowRgn(mBaseHwnd, Rgn,true);
	 //DeleteObject(Rgn);

	 mListener.attachWindow(mBaseHwnd);
	 //TitleBar
	 TitleBar * pBar = new TitleBar(L"pSong's Window", Conver::MyRect(0, 0, mWidth - 3 * 35, 35), RGB(255, 255, 255), RGB(65, 65, 68), RGB(0, 0, 0), RGB(116, 116, 119));
	 mListener.attach(pBar);
	 //Close Button
	 CloseButton * pClo = new CloseButton(L"",Conver::MyRect(1024 - 35, 0, mWidth, 35), RGB(255, 255, 255), RGB(65, 65, 68), RGB(0, 0, 0), RGB(228, 0, 0));
	 mListener.attach(pClo);
	 //Max Button
	 MaxButton * pMax = new MaxButton(L"",Conver::MyRect(1024 - 35 * 2, 0, mWidth - 35,35),  RGB(255, 255, 255), RGB(65, 65, 68), RGB(0, 0, 0),RGB(216, 120, 17));
	 mListener.attach(pMax);
	 //Mini Button
	 MiniButton * pMin = new MiniButton(L"", Conver::MyRect(1024 - 35 * 3, 0, mWidth - 70,35), RGB(255, 255, 255), RGB(65, 65, 68), RGB(0, 0, 0), RGB(216, 120, 17));
	 mListener.attach(pMin);
	 //Test Button
	 Button * ptest = new Button(L"", Conver::MyRect(100, 100, 150, 125), RGB(255, 255, 255), RGB(65, 65, 68), RGB(17, 120, 216), RGB(216, 120, 17));
	 ptest->SetDrag(true);
	 mListener.attach(ptest);
	 //Test EditBox
	 EditBox * pEditBox = new EditBox(L"abcde", Conver::MyRect(200, 200, 300, 225), RGB(255, 255, 255), RGB(65, 65, 68), RGB(17, 120, 216), RGB(216, 120, 17));
	 pEditBox->SetDrag(true);
	 mListener.attach(pEditBox);

 }

 void BaseWindow::OnDraw()
 {
	 DrawManager.Clear(MyColor::Black);

	 mListener.Draw();

	 RECT windowRect;
	 GetWindowRect(mBaseHwnd, &windowRect);
	 Conver::ScreenToClientRc(mBaseHwnd, windowRect);
	 DrawManager.DrawRectangle(windowRect, MyColor::Blue, false);

	 DrawManager.Present(&windowRect);

 }

 void BaseWindow::Update(float delta)
 {
	 POINT pt; GetCursorPos(&pt);

	 ScreenToClient(mBaseHwnd, &pt);
	 mListener.Hover(pt); 	 //hover

	 ReDraw(true);
 }

 void BaseWindow::OnNcPaint(HRGN rgn)
 {
	
 }

 void BaseWindow::OnNcCalcSize(WPARAM wParam,LPARAM lParam)
 {

 }

 bool BaseWindow::OnNcActive(WPARAM wParam, LPARAM lParam)
 {
	 return true;
 }

 UINT BaseWindow::OnSize(WPARAM wParam, LPARAM lParam)
 {
	 auto newWidth  =  LOWORD(lParam);
	 auto newHeight =  HIWORD(lParam);
	 mListener.ChangeSize(Conver::MyRect(mLeftTop.x,mLeftTop.y,mLeftTop.x+newWidth,mLeftTop.y+newHeight));
	 return 0;
 }

 bool BaseWindow::OnSizing(WPARAM wParam, LPARAM lParam)
 {
	 RECT * pNewRect = reinterpret_cast<RECT *>(lParam);
	 mListener.ChangeSize(*pNewRect);
	 return true;
 }

 UINT BaseWindow::OnHitTest(LPARAM lParam)
 {
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y };
	 ScreenToClient(mBaseHwnd, &pt);

	 return mListener.HitTest(pt);
 }

 void BaseWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y };
	 auto ret = mListener.LButtonDown(pt);
	 mMouse.x = pt.x;
	 mMouse.y = pt.y;
	 mMouse.mMouseState = MOUSE_STATE_LEFTBUTTONDOWN;
 }

 void BaseWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
 {
	 static RECT RestoreRect; //保存当前区域
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y };

	 auto ret = mListener.LButtonUp(pt);
	 if (ret == SHOULD_CLOSE_WINDOW) SendMessage(mBaseHwnd,WM_CLOSE,0,0);
	 if (ret == SHOULD_MINI_WINDOW)  ShowWindow(mBaseHwnd, SW_MINIMIZE);
	 if (ret == SHOULD_MAX_WINDOW)
	 {
		 RestoreRect = { mLeftTop.x,mLeftTop.y, mLeftTop.x + GetWidth(),mLeftTop.y + GetHeight() };
		 RECT desRect = Conver::GetMaxSizeRect();
		 SetRect(desRect);
		 DrawManager.ReSize(RECTWIDTH(desRect),RECTHEIGHT(desRect));
	 }

	 if (ret == SHOULD_RESTORE_WINDOW)
	 {
		 SetRect(RestoreRect);
		 DrawManager.ReSize(RECTWIDTH(RestoreRect), RECTHEIGHT(RestoreRect));
	 }

 }

 UINT BaseWindow::OnUnicodeChar(WPARAM wParam, LPARAM lParam)
 {
	 auto pressChar = (wchar_t)wParam;
	 mListener.InputChar(pressChar);
	 return 0;
 }

 void BaseWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
 {
	 auto mousePt =  MAKEPOINTS(lParam);
	 mMouse.x = mousePt.x; mMouse.y = mousePt.y;
	 mListener.MouseMove(Conver::Point(mousePt.x,mousePt.y));
 }




 namespace MOUSE
 {
	 Mouse::Mouse()
	 {
		 x = y = -1;
		 mMouseState = MOUSE_STATE_IDLE;
	 }
 }


 }


