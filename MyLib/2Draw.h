#pragma once
# include "Macros.h"
# include <d2d1.h>
# include <DWrite.h>
#include <windows.h>
#include <wrl.h>

# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"Dwrite.lib")
# pragma comment(lib,"windowscodecs.lib")

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw)
private:
	ID2D1Factory * mFactory;
	ID2D1HwndRenderTarget * mRenderTarget;
	IDWriteFactory * mWriteFactory;
public:
	bool InitManager();
	bool SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect = NULL);
	ID2D1SolidColorBrush * CreateBrush(D2D1::ColorF BrushColor = D2D1::ColorF::Black);
	ID2D1Bitmap * CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth = 0, UINT dstHeight = 0);
	bool DrawRectangle(RECT Rect, bool isFillRectangle = false, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawRectWithText(RECT Rect,std::string text, bool isFillRectangle = false, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawEllipse(POINT centerPoint, float r1, float r2, bool isFillEllipse = false, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawLine(POINT src, POINT des, ID2D1SolidColorBrush * pSoildBrush = NULL);
	bool DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect);
	IDWriteTextLayout * CreateTextLayout(std::string text);
	ID2D1HwndRenderTarget * getRenderTarget() { return mRenderTarget; }
	~My2DDraw();
};

#define DrawManager My2DDraw::getInstance() 
#define MyColor D2D1::ColorF