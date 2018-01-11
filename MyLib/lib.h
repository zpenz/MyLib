# pragma once
# include <stdio.h>
# include <cstdlib>
# include <windows.h>
# pragma warning(disable:4305)
# pragma warning(disable:4996)
#define WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define MAX_KEYS 256
# define SAFE_RELEASE(a) if(a){a->Release(); a=NULL;}
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
		OutputDebugStringA(error_message);\
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

 class  My_Window
{
private:
	int m_width;
	int m_height;
	bool m_isfullscreen;
	const char * m_classname; 
	DWORD mWindowStyleEx;
	DWORD mWindowStyle;
	HINSTANCE mHInstance;
	HBRUSH m_background;
	//额外参数
	void * m_lpvoid;
	Point mLeftTop;
	HDC  m_hdc;
	HWND hBaseWnd; 
	//鼠标位置
	Point mousePos;
	//改变窗口位置
	void ChangeSize();
public:
	const char * mwindowname;
	//仅仅无效化客户区 使其重绘
	void ReDraw();
	Keys key;
	My_Window();
	virtual ~My_Window();
	My_Window(int width, int height, bool isfullscreen, const char * windowname, const char * classname, DWORD WindowStyleEx, DWORD WindowStyle, void * lpvoid, Point leftUpper)
		:m_width(width), m_height(height), m_isfullscreen(isfullscreen), mwindowname(windowname), m_classname(classname),
		mWindowStyle(WindowStyle), mWindowStyleEx(WindowStyleEx) {}
	bool Create(void);
	/*must call before you use Create function*/
	void SetInstance(HINSTANCE hInstance) {this->mHInstance = hInstance;}
	void SetWidth(int Width) {if(Width>0|| Width<WIDTH){this->m_width = Width;}else{this->m_width = 1024;}}
	void SetHeight(int Height) {if(Height>0|| Height<HEIGHT){this->m_height = Height;}else{this->m_height = 768;}}
	void SetWindowName(const char * windowname){this->mwindowname = windowname;}
	void SetClassName(const char * classname){this->m_classname = classname;}
	void SetWindowStyle(DWORD WindowStyle){this->mWindowStyle=WindowStyle;}
	void SetWindowStyleEx(DWORD WindowStyleEx){this->mWindowStyleEx = WindowStyleEx;}
	void SetWindowPos(Point leftUpper){this->mLeftTop = leftUpper;}
	void SetUserData(LPVOID lpvoid){this->m_lpvoid = lpvoid;}
	void SetFullScrren(bool isfullscreen){this->m_isfullscreen = isfullscreen;}
	void SetMousePos(int px,int py) {mousePos.x = px; mousePos.y = py;}
	void SetBrush(const HBRUSH & bs) {this->m_background = bs;}
	//Get function
	int GetWidth(void) {return this->m_width;}
	int GetHeight(void) {return this->m_height;}
	const char * GetWindowName(void){return mwindowname;}

	Point GetMousePos(void){return mousePos;}
	
	bool SetHDC(HDC hDC){this->m_hdc = hDC; return hDC?true:false;}
	HDC  GetHDC(void){return m_hdc;}
	HWND GetHwnd(void){return hBaseWnd;}
	
	virtual void Destory();
	virtual void MessageLoop();
	virtual void OwnOperator(float delta);
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

	operator HDC()  {return this->m_hdc;}
	operator HWND() {return this->hBaseWnd;}
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