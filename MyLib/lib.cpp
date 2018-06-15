# include "lib.h"

namespace LIB_WINDOW
{
	using namespace MutexLock;

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
	 mWindowStyleEx(WindowStyleEx),mLeftTop(leftUpper),mDefaultWrapper(true)
 {
 }

 BaseWindow::BaseWindow(string layoutFileName) : mWidth(1024), mHeight(768), mBaseHwnd(NULL),
	 mClassname(L"LIB"), mWindowname(L"LIB"), mWindowStyle(WS_OVERLAPPEDWINDOW),
	 mWindowStyleEx(WS_EX_APPWINDOW), mLayoutFileName(layoutFileName),mDefaultWrapper(true)
{

}

 BaseWindow::BaseWindow() :BaseWindow("") {}


bool BaseWindow::ShowThisWindow()
{
	using namespace Conver;
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

	mBaseHwnd = CreateWindowExW(mWindowStyleEx, mClassname.c_str(), 
		mWindowname.c_str(),
		mWindowStyle,mLeftTop.x,mLeftTop.y,mWidth,mHeight,NULL,NULL,NULL,this);

	if(!mBaseHwnd)
	{
		ErrorMessage(Conver::Format("创建窗口失败.ErrorCode: %d", GetLastError()).c_str());
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
	if (mBaseHwnd)
		//::MoveWindow(mBaseHwnd, mLeftTop.x, mLeftTop.y, mWidth, mHeight, false);
		::SetWindowPos(mBaseHwnd,NULL,mLeftTop.x,mLeftTop.y,mWidth,mHeight, SWP_NOMOVE);
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

HANDLE BaseWindow::Show()
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
	return hThread;
}

void BaseWindow::UseWrapper()
{
	mDefaultWrapper = false;
}

bool BaseWindow::IsDefaultWrapper()
{
	return mDefaultWrapper;
}

bool BaseWindow::Close()
{
	IS_RETURN(!mBaseHwnd,false);
	SendMessage(mBaseHwnd, WM_CLOSE, 0, 0);;
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
	static vector<UINT> vImportKeyMessage = 
	{
		VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN
	};

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
			if(window->IsDefaultWrapper()) break;
			window->OnNcPaint((HRGN)wParam);
			return 0;
			break;
		case WM_NCCALCSIZE:
			if (window->IsDefaultWrapper()) break;
			window->OnNcCalcSize(wParam,lParam);
			if ((BOOL)wParam) return 0;
			break;
		case WM_NCHITTEST:
			if (window->IsDefaultWrapper()) break;
			return window->OnHitTest(lParam);
		case WM_NCACTIVATE:
			if (window->IsDefaultWrapper()) break;
			return window->OnNcActive(wParam,lParam);
			break;
		case WM_ERASEBKGND: 
			return 1; // 不擦除背景
		case WM_KEYUP:
			break;
		case WM_KEYDOWN:
			if (find_if(vImportKeyMessage.begin(),vImportKeyMessage.end(),[wParam](UINT & uint){
				if (wParam == uint) return true;
				return false;
			}) == vImportKeyMessage.end()) break;
		case WM_CHAR:
		case WM_UNICHAR:
		case WM_IME_CHAR:
			return window->OnUnicodeChar(wParam,lParam);
		case WM_SETCURSOR:
			return true; ///阻止window自动还原cursor shape
			break;
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
	 IS_RETURN_ERROR(!DrawManager.InitManager(),,"初始化D2D错误!");
	 IS_RETURN_ERROR(!DrawManager.SetRenderTarget(mBaseHwnd),,"设置RenderTarget失败!");
	 DrawManager.UseTempRenderTarget();

	 CaretManager.attrach(mBaseHwnd);
	 CaretManager.Color(RGB(0,0,0));

	 //窗口完全矩形..
	 //auto Rgn = CreateRectRgn(0,0,mWidth,mHeight);
	 //SetWindowRgn(mBaseHwnd, Rgn,true);
	 //DeleteObject(Rgn);

	 mListener.attachWindow(mBaseHwnd);
	 //加载所有控件
	 IS_ERROR_EXIT(!Layout::ControlLayout.LoadLayoutFile(mLayoutFileName.empty()?DEFAULT_LAYOUT: mLayoutFileName.c_str(),&mListener),"加载布局文件失败...");
	 auto background = mListener.findElementByID(0);
	 IS_ERROR_EXIT(!background, "没有找到背景控件!");
	 SetWidth(background->width());
	 SetHeight(background->height());
	 DrawManager.ReSize(background->width(),background->height());
 }

 void BaseWindow::OnDraw()
 {
	 DrawManager.Clear(MyColor::Black);
	 mListener.Draw();
	 RECT windowRect;
	 GetWindowRect(mBaseHwnd, &windowRect);
	 Conver::ScreenToClientRc(mBaseHwnd, windowRect);
	 DrawManager.DrawRectangle(windowRect, COLOREX(RGB(57, 130, 255)), false);
	 DrawManager.Present(&windowRect);
 }

 void BaseWindow::Update(float delta)
 {
	 if (mListener.Obj().empty()) return;
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
	 if (mListener.Obj().empty()) return 0;
	 auto newWidth  =  LOWORD(lParam);
	 auto newHeight =  HIWORD(lParam);
	 mListener.ChangeSize(Conver::MyRect(mLeftTop.x,mLeftTop.y,mLeftTop.x+newWidth,mLeftTop.y+newHeight));
	 return 0;
 }

 bool BaseWindow::OnSizing(WPARAM wParam, LPARAM lParam)
 {
	 if (mListener.Obj().empty()) return true;
	 RECT * pNewRect = reinterpret_cast<RECT *>(lParam);
	 mListener.ChangeSize(*pNewRect);
	 return true;
 }

 UINT BaseWindow::OnHitTest(LPARAM lParam)
 {
	 if (mListener.Obj().empty()) return 0;
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y };
	 ScreenToClient(mBaseHwnd, &pt);
	 return mListener.HitTest(pt);
 }

 void BaseWindow::AddClickFuncByID(UINT id, function<void(void)> pFunc, bool rewrite)
 {
	 IS_RETURN_ERROR(mListener.Obj().empty(),,"mListener为空");
	 mListener.AddClickFuncByID(id,pFunc,rewrite);
 }

 Control * BaseWindow::findElementByID(UINT id)
 {
	 IS_RETURN_ERROR(mListener.Obj().empty(), nullptr, "mListener为空");
	 return mListener.findElementByID(id);
 }

 bool BaseWindow::SetValueByID(UINT id, wstring stringValue)
 {
	 IS_RETURN_ERROR(mListener.Obj().empty(),false , "mListener为空");
	 return mListener.SetValueByID(id, stringValue);
 }

 wstring BaseWindow::getValueByID(UINT id)
 {
	 IS_RETURN_ERROR(mListener.Obj().empty(),L"NULL" , "mListener为空");
	 return mListener.getValueByID(id);
 }

 void BaseWindow::SetRangeIDValue(UINT startId, UINT endId, wstring stringValue)
 {
	 IS_RETURN_ERROR(mListener.Obj().empty(), , "mListener为空");
	 mListener.SetRangeIDValue(startId, endId, stringValue);
 }

 void BaseWindow::OnLButtonDown(WPARAM wParam, LPARAM lParam)
 {
	 if (mListener.Obj().empty()) return;
	 POINT pt = { MAKEPOINTS(lParam).x,MAKEPOINTS(lParam).y };
	 mListener.LButtonDown(pt);
	 mMouse.x = pt.x;
	 mMouse.y = pt.y;
	 mMouse.mMouseState = MOUSE_STATE_LEFTBUTTONDOWN;
 }

 void BaseWindow::OnLButtonUp(WPARAM wParam, LPARAM lParam)
 {
	 if (mListener.Obj().empty()) return;
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
	 if (mListener.Obj().empty()) return 0;
	 auto pressAchar = (char)wParam;
	 auto pressChar = (wchar_t)wParam;
	 mListener.InputChar(pressChar);
	 return 0;
 }

 void BaseWindow::OnMouseMove(WPARAM wParam, LPARAM lParam)
 {
	 if (mListener.Obj().empty()) return;
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


