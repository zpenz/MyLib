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



typedef struct PicStruct
{
public:
	RECT mDesRect;

	wchar_t * mFileName;
	float mWidth;
	float mHeight;
	bool mIsHighRender;
	RECT mPicRect;
	bool mIsNeedClip;
	float mPicAlpha;
	ID2D1Bitmap * mPicture;
   
	UINT GetWidth() { return mDesRect.right - mDesRect.left; }
	UINT GetHeight() { return mDesRect.bottom - mDesRect.top; }

	PicStruct(wchar_t * filenmae,RECT desRect, float fAlpha = 1.0,bool bHighRender=true):mFileName(filenmae),mDesRect(desRect)
		,mIsHighRender(bHighRender),mPicAlpha(fAlpha)
	{
	}
	PicStruct() {}
	~PicStruct();

}RenderPicture;

class lib2d : public BaseWindow
{
private:
	ID2D1Factory * pFactory;
	ID2D1HwndRenderTarget * pRenderTarget;
	ID2D1SolidColorBrush       * pBrush;
	ID2D1Bitmap         * pBitmap;
	IWICImagingFactory *  pWICFactory;
	vector<RenderPicture> * pPictureSet;
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

	bool SetBrushColor(float a, float r, float b, float g);
	virtual bool InitResource();
	virtual void ClearBackground(float a, float r, float g, float b);
	virtual void DrawRectangle();
	virtual void DrawRectangle(RECT & rc);
	virtual void DrawRectangle(int left,int top,int right,int bottom);
	virtual void DrawBitmap();
	virtual void DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);

	 HRESULT LoadBitmapFromFile(
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

