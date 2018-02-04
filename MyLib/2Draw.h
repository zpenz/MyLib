#pragma once
# include "Macros.h"
# include <d2d1.h>
# include <DWrite.h>
#include <windows.h>
#include <wrl.h>

# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"Dwrite.lib")
# pragma comment(lib,"windowscodecs.lib")

//¶ÔÆë·½Ê½
#define ALIGN_DEFAULT    0
#define ALIGN_LEFT       1
#define ALIGN_RIGHT      2
#define ALIGN_TOP        3
#define ALIGN_BOTTOM     4
#define ALIGN_CENTER_V   5
#define ALIGN_CENTER_H   6

#define MyColor D2D1::ColorF

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw)
private:
	ID2D1Factory * mFactory;
	ID2D1RenderTarget * mRenderTarget;
	IDWriteFactory * mWriteFactory;
public:
	bool InitManager();

	bool SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect = NULL);

	bool SetRenderTarget(HDC hDC, RECT * pRc);

	ID2D1SolidColorBrush * CreateBrush(MyColor BrushColor = MyColor::Black);

	bool DrawRectangle(RECT Rect, MyColor RectColor, bool isFillRectangle = false);

	bool DrawRectWithText(RECT Rect,std::string text, MyColor RectColor, MyColor TextColor,UINT AlignType= ALIGN_DEFAULT,bool isFillRectangle = false);

	bool DrawEllipse(POINT centerPoint, float r1, float r2, MyColor EllipseColor,bool isFillEllipse = false);

	bool DrawLine(POINT src, POINT des, ID2D1SolidColorBrush * pSoildBrush = NULL);

	bool DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect);

	bool Clear(MyColor color = MyColor::Black);

	IDWriteTextLayout * CreateTextLayout(std::string text);

	ID2D1Bitmap * CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth = 0, UINT dstHeight = 0);

	ID2D1RenderTarget * getRenderTarget() { return mRenderTarget; }

	~My2DDraw();
};

#define DrawManager My2DDraw::getInstance() 

