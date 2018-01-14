#pragma once
# include "lib.h"
# include <d2d1.h>
# include <wincodec.h>
# include <vector>
#include <iostream>
//ComPtr
#include <wrl.h>
using namespace Microsoft::WRL;

using namespace std;
# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"windowscodecs.lib")
# pragma warning(disable:4244)
# pragma warning(disable:4172)
//×ª»»
D2D1_RECT_F & RectToD2DRectF(RECT & rc);

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw);
private:
	ID2D1Factory * mFactory;
	ID2D1HwndRenderTarget * mRenderTarget;
public:
	int a;
	void test() { std::cout << a << std::endl; }
	bool InitManager();
	bool SetRenderTarget(_In_ HWND hTargetWindowHwnd, RECT * pRect = NULL);
	ID2D1SolidColorBrush * CreateBrush(D2D1::ColorF BrushColor = D2D1::ColorF::Black);
	ID2D1Bitmap * CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth = 0, UINT dstHeight = 0);
	bool DrawRectangle(_In_ RECT Rect, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawLine(POINT src, POINT des, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect);
	ID2D1HwndRenderTarget * getRenderTarget() { return mRenderTarget; }
};

#define DrawManager My2DDraw::getInstance() 
#define MyColor D2D1::ColorF

typedef struct rp
{
public:
	RECT mDesRect;

	wchar_t * mFileName;
	float sw_width;
	float sw_height;
	bool mbHighRender;
	RECT sw_rc;
	bool need_clip;
	float alpha;
	ID2D1Bitmap * pBitmap;
   
	UINT GetWidth() { return mDesRect.right - mDesRect.left; }
	UINT GetHeight() { return mDesRect.bottom - mDesRect.top; }

	rp(wchar_t * filenmae,RECT desRect, float fAlpha = 1.0,bool bHighRender=true):mFileName(filenmae),mDesRect(desRect)
		,mbHighRender(bHighRender),alpha(fAlpha)
	{
	}
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
	vector<Render_Bitmap> * pPictureSet;
	void cleanup();
	D2D1_COLOR_F brushcolor;
	void SetBitmapResource();
public:
	
	bool AddBitmap(wchar_t * pic_name,RECT desRect);
	bool AddBitmap(wchar_t * pic_name, RECT desRect,int shw_width,int shw_height);
	bool AddBitmap(wchar_t * pic_name, RECT desRect,float alpha,bool is_high_render,RECT sw_rc);
	bool AddBitmap(wchar_t * pic_name, RECT desRect, float alpha, bool is_high_render, RECT sw_rc,int sw_width,int sw_height);

	virtual void Destory();
	virtual void OnDraw() ;
	virtual void Draw();
	virtual void AfterCreate();

	bool SetBrushColor(int a, int r, int b, int g);
	virtual bool InitResource();
	virtual void ClearBackground(int a,int r,int g,int b);
	virtual void DrawRectangle();
	virtual void DrawRectangle(RECT & rc);
	virtual void DrawRectangle(int left,int top,int right,int bottom);
	virtual void DrawBitmap();
	virtual void DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);

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

