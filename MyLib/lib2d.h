#pragma once
# include "lib.h"
# include <d2d1.h>
# include <wincodec.h>
# include <vector>
#include <iostream>
using namespace std;
# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"windowscodecs.lib")
# pragma warning(disable:4244)
# pragma warning(disable:4172)
//转换
D2D1_RECT_F & RectToD2DRectF(RECT & rc);

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw);
private:
	ID2D1Factory * mFactory;
	ID2D1HwndRenderTarget * mRenderTarget;
public:
	int a;
	void test() { std::cout << a<<std::endl; }
	bool InitManager();
	bool SetRenderTarget(_In_ HWND hTargetWindowHwnd,RECT * pRect=NULL);
	ID2D1SolidColorBrush * CreateBrush(D2D1::ColorF BrushColor = D2D1::ColorF::Black);
	bool DrawRectangle(_In_ RECT Rect, ID2D1SolidColorBrush * pSoildBrush = NULL);
};

#define DrawManager My2DDraw::getInstance() 
#define MyColor D2D1::ColorF

//位图渲染结构
typedef struct rp
{
	//图片名称
	wchar_t * url;
	//所需要位图宽度,高度
	//如果都为0 则使用原图
	float need_width;
	float need_height;
	//左上角位置
	float left_pos;
	float top_pos; 
	//显示宽度和高度
	float sw_width;
	float sw_height;
	//渲染线性插值还是近似处理？
	bool is_high_render;
	//从原图选择一部分渲染
	RECT sw_rc;
	//是否需要剪切
	bool need_clip;
	//不透明度
	float alpha;
	//位图指针
	ID2D1Bitmap * pBitmap;
	rp();
	~rp();

}Render_Bitmap;

class lib2d : public My_Window
{
private:
	ID2D1Factory * pFactory;
	ID2D1HwndRenderTarget * pRenderTarget;
	ID2D1SolidColorBrush       * pBrush;
	ID2D1Bitmap         * pBitmap;
	IWICImagingFactory *  pWICFactory;
	vector<Render_Bitmap> * v_bitmap;
	//释放资源
	void cleanup();
	D2D1_COLOR_F brushcolor;
	//载入位图资源
	void LoadBitmapResource();
public:
	
	//添加位图
	bool AddBitmap(wchar_t * pic_name,int des_width, int des_height,int pos_x,int pos_y);
	bool AddBitmap(wchar_t * pic_name,int des_width,int des_height,int pos_x,int pos_y,int shw_width,int shw_height);
	//AddBitmap包含了图片名字 宽度 高度 渲染位置 不透明度 是否高质量渲染 显示原图的哪个区域的内容
	bool AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y,float alpha,bool is_high_render,RECT sw_rc);
	//AddBitmap包含了图片名字 宽度 高度 渲染位置 不透明度 是否高质量渲染 显示原图的哪个区域的内容 显示宽度 显示高度
	bool AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc,int sw_width,int sw_height);
	//AddBitmap包含了图片名字 宽度 高度 渲染位置 不透明度 是否高质量渲染 显示原图的哪个区域的内容 显示宽度 显示高度(des_width和des_height没用)
	bool AddBitmap(wchar_t * pic_name, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc, int sw_width, int sw_height);

	virtual void Destory();
	virtual void OnCreate();
	virtual void Init();
	virtual void OnDraw() ;
	virtual void Draw();
	virtual void AfterCreate();
	virtual void  Clear();
	
	//初始化资源
	virtual bool InitResource();
	//设置画刷颜色
	bool SetBrushColor(int a,int r,int b,int g);
	//清除背景
	virtual void ClearBackground(int a,int r,int g,int b);
	virtual void DrawRectangle();
	virtual void DrawRectangle(RECT & rc);
	virtual void DrawRectangle(int left,int top,int right,int bottom);
	virtual void DrawBitmap();
	virtual void DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);
	virtual void DrawBitmap2(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);

	virtual HRESULT LoadBitmapFromFile(
		ID2D1RenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR uri,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
		);
	lib2d();
	~lib2d();
};

