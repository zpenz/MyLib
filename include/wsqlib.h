
# pragma once

/*------------------------define this to exprot our function whitout thinking about the special name of function-------------------------------------------*/
# define EXPORT_F extern "C" _declspec(dllexport) 
# define EXPORT  _declspec(dllexport) 
# define SAFE_Release(a) if(a){a->Release(); a=NULL;}

# include <stdio.h>
# include <cstdlib>

# include <windows.h>
# pragma warning(disable:4305)
# pragma warning(disable:4996)
  
 
#define WIDTH  GetSystemMetrics(SM_CXSCREEN)
#define HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define MAX_KEYS 256

//#define WSQ_DEBUG

extern void Error_Box(const char * _error);

typedef struct 
{
	int x;
	int y;
}Point;

/*
================
some convince function
MessageBox
================
*/        

//按键类
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
	//是否按下
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
	int width;
	int height;

	bool isfullscreen;

	
	const char * classname; 

	DWORD WindowStyleEx;
	DWORD WindowStyle;
	HINSTANCE hInstance;
	HBRUSH background;

	//额外参数
	void * lpvoid;

	Point left_Upper;
	HDC  m_hdc;
	HWND hWnd;

	//鼠标位置
	Point mousePos;

	//计算坐标
	void ChangeSize(void);

public:
	
	const char * windowname;
	//ReDraw Window use default backgroud color. but don not use it to clear the game screen, it dont do well.
	void ReDraw();

	Keys key;
	/*默认的构造函数*/ 
	My_Window();

	/*析构函数*/
	virtual ~My_Window();

	/* if all need to devise please call it*/
	My_Window(int width,int height,bool isfullscreen ,const char * windowname,const char * classname,DWORD WindowStyleEx,DWORD WindowStyle,void * lpvoid,Point leftUpper);

	bool Create(void);

	/*Please call the Set function before you use Create function*/
	void SetInstance(HINSTANCE hInstance) {this->hInstance = hInstance;}
	void SetWidth(int Width) {if(Width>0|| Width<WIDTH){this->width = Width;}else{this->width = 1024;}}
	void SetHeight(int Height) {if(Height>0|| Height<HEIGHT){this->height = Height;}else{this->height = 768;}}
	void SetWindowName(const char * windowname){this->windowname = windowname;}
	void SetClassName(const char * classname){this->classname = classname;}
	void SetWindowStyle(DWORD WindowStyle){this->WindowStyle=WindowStyle;}
	void SetWindowStyleEx(DWORD WindowStyleEx){this->WindowStyleEx = WindowStyleEx;}
	void SetWindowPos(Point leftUpper){this->left_Upper = leftUpper;}
	void SetUserData(LPVOID lpvoid){this->lpvoid = lpvoid;}
	void SetFullScrren(bool isfullscreen){this->isfullscreen = isfullscreen;}
	void SetMousePos(int px,int py) {mousePos.x = px; mousePos.y = py;}
	void SetBrush(const HBRUSH & bs) {this->background = bs;}

	//Get function
	int GetWidth(void) {return this->width;}
	int GetHeight(void) {return this->height;}
	const char * GetWindowName(void){return windowname;}

	Point GetMousePos(void){return mousePos;}
	
	bool SetHDC(HDC hDC){this->m_hdc = hDC; return hDC?true:false;}
	HDC  GetHDC(void){return m_hdc;}
	HWND GetHwnd(void){return hWnd;}
	
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
	operator HWND() {return this->hWnd;}

	/*virtual LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);*/
	friend 	
		LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};


/*
===================
All need project 
   start with '$' or '#'
   others need to fly out.
===================
*/
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

# ifdef WSQ3D_H
# include "wsqd3d.h"
# endif