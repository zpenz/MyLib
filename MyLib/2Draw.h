/************************************************************************/
/* D2D1.0 Draw															*/
/************************************************************************/

#pragma once
# include "Macros.h"
# include <d2d1.h>
# include <DWrite.h>
#include <windows.h>
#include <wrl.h>

# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"Dwrite.lib")
# pragma comment(lib,"windowscodecs.lib")

using namespace Macros2D;

#define MyColor D2D1::ColorF
#define IPIC    ID2D1Bitmap 
#define TextLayout IDWriteTextLayout
#define HitTestMatric DWRITE_HIT_TEST_METRICS

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw)
private:
	ID2D1Factory * mFactory;

	ID2D1RenderTarget * mRenderTarget;

	ID2D1HwndRenderTarget * mHwndRenderTarget;

	ID2D1BitmapRenderTarget * mTempTarget;

	IDWriteFactory * mWriteFactory;
public:

	bool InitManager();

	bool SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect = NULL);

	bool UseTempRenderTarget(); // "double" buffer

	bool SetCurrentRenderTarget(ID2D1RenderTarget * thisRenderTarget);

	ID2D1SolidColorBrush * CreateBrush(MyColor BrushColor = MyColor::Black);

	bool DrawRectangle(RECT Rect, MyColor RectColor, bool isFillRectangle = false);

	bool DrawText(std::string text, RECT layoutBox,MyColor TextColor,float fontSize,_Out_ TextLayout ** pLayout, ALIGN_TEXT_TYPE textAlignType = ALIGN_TEXT_CENTER, ALIGN_PARAGRAPH_TYPE paragraphAlignType = ALIGN_PARAGRAPH_CENTER);

	bool DrawTextW(std::wstring text, RECT layoutBox, MyColor TextColor, float fontSize, _Out_ TextLayout ** pLayout, ALIGN_TEXT_TYPE textAlignType = ALIGN_TEXT_CENTER, ALIGN_PARAGRAPH_TYPE paragraphAlignType = ALIGN_PARAGRAPH_CENTER);

	bool DrawRectWithText(RECT Rect, std::string text, MyColor RectColor, MyColor TextColor, _Out_ TextLayout ** pLayout,bool isFillRectangle = false,ALIGN_TEXT_TYPE textAlignType = ALIGN_TEXT_CENTER, ALIGN_PARAGRAPH_TYPE paragraphAlignType = ALIGN_PARAGRAPH_CENTER);

	bool DrawRectWithTextW(RECT Rect, std::wstring text, MyColor RectColor, MyColor TextColor, _Out_ TextLayout ** pLayout, bool isFillRectangle = false, ALIGN_TEXT_TYPE textAlignType = ALIGN_TEXT_CENTER, ALIGN_PARAGRAPH_TYPE paragraphAlignType = ALIGN_PARAGRAPH_CENTER);

	bool DrawRectWithPicture(RECT Rect, MyColor RectColor, std::wstring strFileName, UINT uOffSetX = 0,UINT uOffSetY = 0,bool isFillRectangle = false);

	bool DrawEllipse(POINT centerPoint, float r1, float r2, MyColor EllipseColor,bool isFillEllipse = false);

	bool DrawLine(POINT src, POINT des, MyColor lineColor);

	bool DrawLine(POINT src, POINT des, MyColor lineColor,float LineWidth);

	bool DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect);

	bool DrawPicture(std::wstring strFileName, RECT decRect);

	bool DrawPicture(std::wstring strFileName, RECT decRect, RECT srcRect);

	ID2D1Layer * CreateLayer(ID2D1RenderTarget * thisRenderTarget);

	void PushLayer(ID2D1Layer * layer);

	void PopLayer(ID2D1Layer * layer);

	bool ReSize(UINT uWidth, UINT uHeight); //resize uWidth:新宽度 uHeight:新高度 

	bool Clear(MyColor color = MyColor::Black);

	bool Present(RECT * pRect); // present temp render target

	IDWriteTextLayout * CreateTextLayout(std::string text); // default size 18.0f

	IDWriteTextLayout * CreateTextLayout(std::string text,float fSize);

	IDWriteTextLayout * CreateTextLayoutW(std::wstring text); 

	IDWriteTextLayout * CreateTextLayoutW(std::wstring text, float fSize);

	/// pClipRect 裁剪区域 从图片的左上角开始 不能超过图片区域 否则结果是未定义的
	ID2D1Bitmap * CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth = 0, UINT dstHeight = 0,RECT * pClipRect = nullptr); 

	ID2D1RenderTarget * getRenderTarget();

	~My2DDraw();
};

#define DrawManager My2DDraw::getInstance() 

