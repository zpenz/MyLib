# pragma once
# include <stdio.h>
# include <cstdlib>
# include <windows.h>

# pragma warning(disable:4305)
# pragma warning(disable:4996)

#define SCREEN_WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define MAX_KEYS 256
# define SAFE_RELEASE(p) if(p){p->Release(); p=NULL;}

extern void Error_Box(const char * _error);
typedef struct { int x; int y; }Point;

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
		
class Keys
{
private:
	bool keys[MAX_KEYS];
	bool LButtonDown;
	bool LButtonUp;
	bool RButtonDown;
	bool RButtonDUp;
public:
	void SetPress(unsigned int key);
	void SetRelease(unsigned int key);
	bool IsPress(unsigned int key);
	bool isLDown(void){ return LButtonDown?true:false;}
	bool isRDown(void){ return RButtonDown?true:false;}
	void SetLDown(bool flag){ LButtonDown = flag;}
	void SetRDown(bool flag){ RButtonDown = flag;}
	Keys();
	~Keys();
};

 class  BaseWindow
{
private:

	int mWidth;
	int mHeight;
	bool mIsfullscreen;
	const char * mClassname; 
	DWORD mWindowStyleEx;
	DWORD mWindowStyle;
	HINSTANCE mHInstance;
	HBRUSH mBackground;

	//额外参数
	void * mLpvoid;
	Point mLeftTop;

	HDC  mHdc;
	HWND mHBaseWnd; 

	Point mMousePos;

	//Center
	void PutCenterWindow();
public:
	const char * mwindowname;
	//仅仅无效化客户区 使其重绘
	void ReDraw();
	BaseWindow();
	virtual ~BaseWindow();
	BaseWindow(int width, int height, bool isfullscreen, const char * windowname, const char * classname, DWORD WindowStyleEx, DWORD WindowStyle, void * lpvoid, Point leftUpper)
		:mWidth(width), mHeight(height), mIsfullscreen(isfullscreen), mwindowname(windowname), mClassname(classname),
		mWindowStyle(WindowStyle), mWindowStyleEx(WindowStyleEx) {}
	bool Create(void);
	/*must call before you use Create function*/
	void SetInstance(HINSTANCE hInstance) {this->mHInstance = hInstance;}
	void SetWidth(int Width) {if(Width>0|| Width<SCREEN_WIDTH){this->mWidth = Width;}else{this->mWidth = 1024;}}
	void SetHeight(int Height) {if(Height>0|| Height<SCREEN_HEIGHT){this->mHeight = Height;}else{this->mHeight = 768;}}
	void SetWindowName(const char * windowname){this->mwindowname = windowname;}
	void SetClassName(const char * classname){this->mClassname = classname;}
	void SetWindowStyle(DWORD WindowStyle){this->mWindowStyle=WindowStyle;}
	void SetWindowStyleEx(DWORD WindowStyleEx){this->mWindowStyleEx = WindowStyleEx;}
	void SetWindowPos(Point leftUpper){this->mLeftTop = leftUpper;}
	void SetUserData(LPVOID lpvoid){this->mLpvoid = lpvoid;}
	void SetFullScrren(bool isfullscreen){this->mIsfullscreen = isfullscreen;}
	void SetMousePos(int px,int py) {mMousePos.x = px; mMousePos.y = py;}
	void SetBrush(const HBRUSH & bs) {this->mBackground = bs;}
	//Get function
	int GetWidth(void) {return this->mWidth;}
	int GetHeight(void) {return this->mHeight;}
	const char * GetWindowName(void){return mwindowname;}

	Point GetMousePos(void){return mMousePos;}
	
	bool SetHDC(HDC hDC){this->mHdc = hDC; return hDC?true:false;}
	HDC  GetHDC(void){return mHdc;}
	HWND GetHwnd(void){return mHBaseWnd;}
	
	virtual void Destory();
	virtual void MessageLoop();
	virtual void Update(float delta);
	virtual void Init();
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

	operator HDC()  {return this->mHdc;}
	operator HWND() {return this->mHBaseWnd;}
	/*virtual LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);*/
	friend 	
		LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

//===================
//All need 
//   start with '$' or '#'
//===================
class FileLoad
{
private:
	const char * filename;
	FILE * pfile ;
	int height;
	int width;
	int isfullscreen;
	Point left_point; 
public:
	FileLoad(){filename = "config.cfg"; pfile = fopen(filename,"r+");}
	FileLoad(const char * filename) {this->filename = filename;pfile = fopen(filename,"r+");}
	~FileLoad(){free(pfile);}
	bool SearchToken(const char * _token);
};



# ifdef LIB3D9
# include "libD3D.h"
# endif