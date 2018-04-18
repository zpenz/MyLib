#include "2Draw.h"
#include <DWrite.h>
#include <wincodec.h>
#include <memory>

using namespace Conver;

bool My2DDraw::ReSize(UINT uWidth, UINT uHeight)
{
	IS_RETURN_ERROR(mHwndRenderTarget==nullptr,false,"HwndRenderTarget为空");
	auto ret = mHwndRenderTarget->Resize(D2D1::SizeU(uWidth,uHeight));
	if(mTempTarget) UseTempRenderTarget();
	return SUCCEEDED(ret);
}

bool My2DDraw::InitManager()
{
	mFactory = NULL;
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &mFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateFactory失败");
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),(IUnknown **)&mWriteFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateWriteFactory失败");
	return true;
}

bool My2DDraw::SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect)
{
	IS_RETURN_ERROR(!hTargetWindowHwnd, false, "hTargetWindowHwnd不存在");

	IS_RETURN_ERROR(!mFactory, false, "mFactory null");
	if (mRenderTarget != nullptr) SAFE_RELEASE(mRenderTarget); //clean

	RECT tempRect;
	if (pRect == NULL)
	{
		GetWindowRect(hTargetWindowHwnd, &tempRect);
	}
	auto width = tempRect.right - tempRect.left;
	auto height = tempRect.bottom - tempRect.top;

	auto tp = D2D1::RenderTargetProperties();
	auto hr = mFactory->CreateHwndRenderTarget(
		tp,
		D2D1::HwndRenderTargetProperties(
			hTargetWindowHwnd,
			D2D1::SizeU((int)width, (int)height)
		),&mHwndRenderTarget);

	IS_RETURN_ERROR(FAILED(hr), false, "创建HWNDRenderTarget出错!");

	///<warning>short-cut!</warning>
	mRenderTarget = mHwndRenderTarget;
	
	if (FAILED(hr)) return false;
	mRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	return true;
}

bool My2DDraw::UseTempRenderTarget()
{
	IS_RETURN_ERROR(!mFactory, false, "mFactory null");
	IS_RETURN_ERROR(!mHwndRenderTarget, false, "mHwndRenderTarget null");

	auto hr = mHwndRenderTarget->CreateCompatibleRenderTarget(&mTempTarget);
	if (FAILED(hr)) return false;

	mRenderTarget = mTempTarget;
	return true;
}

bool My2DDraw::SetCurrentRenderTarget(ID2D1RenderTarget * thisRenderTarget)
{
	if (!thisRenderTarget) return false;
	mRenderTarget = thisRenderTarget; 
	return true;
}

ID2D1SolidColorBrush * My2DDraw::CreateBrush(MyColor penColor)
{
	ID2D1SolidColorBrush * pSolidBrush;
	IS_RETURN_ERROR(!mRenderTarget,nullptr,"mRenderTarget null!");
	auto hr = mRenderTarget->CreateSolidColorBrush(penColor, &pSolidBrush);
	if (SUCCEEDED(hr)) return pSolidBrush;
	return NULL;
}

ID2D1Bitmap * My2DDraw::CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth, UINT dstHeight,RECT * pClipRect)
{
	ID2D1Bitmap * pD2DBitmap = nullptr;

	CoInitialize(NULL);

	IWICImagingFactory * pImagingFactory = nullptr;

	auto hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pImagingFactory));
	IS_RETURN_ERROR(FAILED(hr), nullptr, "创建IWICImagingFactory接口Instance失败!");

	IWICBitmapDecoder * pDecoder = nullptr;

	hr = pImagingFactory->CreateDecoderFromFilename(
		BitmapFileName,
		NULL,
		GENERIC_READ, //no need write
		WICDecodeMetadataCacheOnLoad,
		&pDecoder);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "从图片文件创建解码器失败!");

	IWICBitmapFrameDecode * pFrameDecode = nullptr;
	hr = pDecoder->GetFrame(0, &pFrameDecode); IS_RETURN_ERROR(FAILED(hr), nullptr, "获取第一frame图片失败!");
	
	UINT oldSizeWidth = 0, oldSizeHeight = 0;
	pFrameDecode->GetSize(&oldSizeWidth, &oldSizeHeight);

	IWICFormatConverter * pFormatConverter = nullptr;
	hr = pImagingFactory->CreateFormatConverter(&pFormatConverter); IS_RETURN_ERROR(FAILED(hr), nullptr, "创建FormatConverter失败");
	
	IWICBitmapScaler * pScaler = nullptr; 
	IWICBitmap * pWICBitmap = nullptr;
	if (dstHeight == 0 && dstWidth == 0)  //in case width and height = 0
	{
		hr = pFormatConverter->Initialize(pFrameDecode,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}
	else
	{
		///in case width = 0 or height = 0
		if (dstWidth == 0)
		{
			FLOAT scalar = static_cast<FLOAT>(dstHeight) / static_cast<FLOAT>(oldSizeHeight);
			dstWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(oldSizeWidth));
		}
		else if (dstHeight == 0)
		{
			FLOAT scalar = static_cast<FLOAT>(dstWidth) / static_cast<FLOAT>(oldSizeWidth);
			dstHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(dstHeight));
		}
		///

		hr = pImagingFactory->CreateBitmapScaler(&pScaler); IS_RETURN_ERROR(FAILED(hr), nullptr, "创建缩放Scaler失败!");
		
		hr = pScaler->Initialize(pFrameDecode, dstWidth, dstHeight, WICBitmapInterpolationModeCubic); IS_RETURN_ERROR(FAILED(hr), nullptr, "初始化缩放Scaler失败!");
	
		hr = pFormatConverter->Initialize(pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}

	IS_RETURN_ERROR(FAILED(hr), nullptr, "Initialize失败!");

	if(pClipRect != nullptr)
	{
		pImagingFactory->CreateBitmapFromSourceRect(pFormatConverter, pClipRect->left, pClipRect->top, RECTWIDTH((*pClipRect)), RECTHEIGHT((*pClipRect)), &pWICBitmap);
		IS_RETURN_ERROR(FAILED(hr), nullptr, "Initialize失败!");
		hr = mRenderTarget->CreateBitmapFromWicBitmap(pWICBitmap, NULL, &pD2DBitmap);
	}
	else
	hr = mRenderTarget->CreateBitmapFromWicBitmap(pFormatConverter,NULL,&pD2DBitmap);

	std::shared_ptr<void> pExit(NULL, [&](void *) {
		CoUninitialize();
		SAFE_RELEASE(pScaler);
		SAFE_RELEASE(pFormatConverter);
		SAFE_RELEASE(pFrameDecode);
		SAFE_RELEASE(pDecoder);
		SAFE_RELEASE(pImagingFactory);
	});

	auto time = GetTickCount();
	if (SUCCEEDED(hr)) return pD2DBitmap;

	time = GetTickCount() - time;
	IS_ERROR_EXIT(1,Format("CreateBitmap失败 timeout: %s",time).c_str());
	return nullptr;
}

ID2D1RenderTarget * My2DDraw::getRenderTarget()
{
	return mRenderTarget; 
}

bool My2DDraw::DrawRectangle(RECT Rect, MyColor RectColor, bool isFillRectangle, float boardSize)
{
	IS_RETURN_ERROR(mRenderTarget==nullptr,false,"RenderTarget为空");
	auto tempSolidBrush = CreateBrush(RectColor);

	mRenderTarget->BeginDraw();
	D2D1_RECT_F desRect = MyRect(Rect);
	if (!isFillRectangle)
		mRenderTarget->DrawRectangle(desRect, tempSolidBrush,boardSize);
	else
		mRenderTarget->FillRectangle(desRect, tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();

	SAFE_RELEASE(tempSolidBrush);
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawTextW(std::string text,IDWriteTextLayout * pTextLayout, MyColor TextColor, RECT layoutBox)
{
	if (text.empty()) return true;
	auto tempBrush = CreateBrush(TextColor);
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(layoutBox)), pTextLayout, tempBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	auto ret = mRenderTarget->EndDraw();
	IS_RETURN_ERROR(FAILED(ret), false, "DrawTextW failed!");
	SAFE_RELEASE(tempBrush);
	return true;
}

IDWriteTextLayout * My2DDraw::CreateTextLayout(std::string text)
{
	return CreateTextLayout(text,18.0f);
}

IDWriteTextLayout * My2DDraw::CreateTextLayout(std::string text, float fSize)
{
	auto wstrDesText = Conver::ACharToWChar(const_cast<char *>(text.c_str()));
	return CreateTextLayoutW(wstrDesText);
}

IDWriteTextLayout * My2DDraw::CreateTextLayoutW(std::wstring text)
{
	return CreateTextLayoutW(text, 18.0f);
}

IDWriteTextLayout * My2DDraw::CreateTextLayoutW(std::wstring text, float fSize)
{
	if (text.empty()) return nullptr;
	IDWriteTextFormat * tempTextFormat = NULL;
	auto hr = mWriteFactory->CreateTextFormat(L"楷体", NULL, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSize, L"en-us", &tempTextFormat);

	IDWriteTextLayout * tempTextLayout = NULL;
	hr = mWriteFactory->CreateTextLayout(text.c_str(), text.length(), tempTextFormat, 100.0f, 0.0f,
		&tempTextLayout);
	IS_ERROR_EXIT(FAILED(hr), "CreateTextLayoutW失败!");
	SAFE_RELEASE(tempTextFormat);
	return tempTextLayout;
}

bool My2DDraw::DrawRectWithText(RECT Rect, std::string text,MyColor RectColor, MyColor TextColor, TextLayout ** pLayout,bool isFillRectangle,ALIGN_TEXT_TYPE textAlignType,ALIGN_PARAGRAPH_TYPE paragraphAlignType)
{
	auto wstrDesText = Conver::ACharToWChar(const_cast<char *>(text.c_str()));
	return DrawRectWithTextW(Rect,wstrDesText,RectColor,TextColor,pLayout,isFillRectangle,textAlignType,paragraphAlignType);
}

bool My2DDraw::DrawRectWithTextW(RECT Rect, std::wstring text, MyColor RectColor, MyColor TextColor, TextLayout ** pLayout, bool isFillRectangle, ALIGN_TEXT_TYPE textAlignType, ALIGN_PARAGRAPH_TYPE paragraphAlignType)
{
	if (*pLayout != nullptr) (*pLayout)->Release();
	if (!DrawRectangle(Rect, RectColor, isFillRectangle)) return false;
	if (text.empty()) return true;

	IDWriteTextLayout * tempTextLayout = CreateTextLayoutW(text);

	///<LayOutBox>MaxWidth-MaxHeight</LayOutBox>
	tempTextLayout->SetMaxWidth(STCAST(FLOAT,RECTWIDTH(Rect)));
	tempTextLayout->SetMaxHeight(STCAST(FLOAT,RECTHEIGHT(Rect)));
	tempTextLayout->SetTextAlignment(textAlignType);
	tempTextLayout->SetParagraphAlignment(paragraphAlignType);

	DWRITE_TRIMMING Trim = { DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
	tempTextLayout->SetTrimming(&Trim, NULL);

	*pLayout = tempTextLayout;

	auto tempBrush = CreateBrush(TextColor);
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(Rect)), tempTextLayout, tempBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	auto ret = mRenderTarget->EndDraw();

	SAFE_RELEASE(tempBrush);
	return true;
}

bool My2DDraw::DrawText(std::string text, RECT layoutBox, MyColor TextColor, float fontSize, TextLayout ** pLayout, ALIGN_TEXT_TYPE textAlignType, ALIGN_PARAGRAPH_TYPE paragraphAlignType)
{
	auto wstrDesText = Conver::ACharToWChar(COCAST(char *,text.c_str()));
	return DrawTextW(wstrDesText, layoutBox,TextColor,fontSize,pLayout,textAlignType,paragraphAlignType);
}

bool My2DDraw::DrawTextW(std::wstring text, RECT layoutBox, MyColor TextColor, float fontSize, TextLayout ** pLayout, ALIGN_TEXT_TYPE textAlignType, ALIGN_PARAGRAPH_TYPE paragraphAlignType)
{
	SAFE_RELEASE((*pLayout));
	if (text.empty()) return true;
	IDWriteTextLayout * tempTextLayout = CreateTextLayoutW(text);

	tempTextLayout->SetMaxWidth(STCAST(FLOAT,RECTWIDTH(layoutBox)));
	tempTextLayout->SetMaxHeight(STCAST(FLOAT, RECTHEIGHT(layoutBox)));
	tempTextLayout->SetTextAlignment(textAlignType);
	tempTextLayout->SetParagraphAlignment(paragraphAlignType);
	
	DWRITE_TEXT_RANGE textRange = { 0, text.length() };
	tempTextLayout->SetFontSize(fontSize, textRange);
	*pLayout = tempTextLayout;

	auto tempBrush = CreateBrush(TextColor);
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(layoutBox)), tempTextLayout, tempBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	auto ret = mRenderTarget->EndDraw();
	IS_RETURN_ERROR(FAILED(ret),false,"DrawTextW failed!");

	SAFE_RELEASE(tempBrush);
	return true;
}

///<OffSet>内边距</OffSet>
bool My2DDraw::DrawRectWithPicture(RECT Rect, MyColor RectColor, std::wstring strFileName, UINT uOffSetX , UINT uOffSetY, bool isFillRectangle)
{
	if(!DrawRectangle(Rect, RectColor, isFillRectangle)) return false;
	if (strFileName.empty()) return true;
	auto dX = static_cast<LONG>(uOffSetX);
	auto dY = static_cast<LONG>(uOffSetY);
	RECT picRect = {Rect.left - dX,Rect.top + dY,Rect.right - dX,Rect.bottom - dY };
	if (!RectInRect(picRect, Rect)) picRect = Rect;
	auto ret = DrawPicture(CreateBitmap(const_cast<wchar_t *>(strFileName.c_str())), picRect);
	return ret;
}

bool My2DDraw::DrawEllipse(POINT centerPoint, float r1, float r2, MyColor EllipseColor,bool isFillEllipse)
{
	auto tempSolidBrush = CreateBrush(EllipseColor);

	mRenderTarget->BeginDraw();
	if (!isFillEllipse)
		mRenderTarget->DrawEllipse(D2D1::Ellipse(PointToD2DPointF(centerPoint), r1, r2), tempSolidBrush);
	else
		mRenderTarget->FillEllipse(D2D1::Ellipse(PointToD2DPointF(centerPoint), r1, r2), tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();

	SAFE_RELEASE(tempSolidBrush);
	return SUCCEEDED(hr);

}

bool My2DDraw::DrawLine(POINT src, POINT des, MyColor lineColor)
{
	return DrawLine(src, des, lineColor, 1.0f);
}

bool My2DDraw::DrawLine(POINT src, POINT des, MyColor lineColor, float LineWidth)
{
	auto tempSolidBrush = CreateBrush(lineColor);
	if (tempSolidBrush == NULL) return false;
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawLine(PointToD2DPointF(src), PointToD2DPointF(des), tempSolidBrush,LineWidth);
	auto hr = mRenderTarget->EndDraw();

	SAFE_RELEASE(tempSolidBrush);
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect)
{
	IS_ERROR_EXIT(!mRenderTarget,"mRenderTarget为空");
	IS_ERROR_EXIT(!pBitmap, "位图结构为空");
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawBitmap(pBitmap, RectToD2DRectF(decRect));
	auto hr = mRenderTarget->EndDraw();
	
	//SAFE_RELEASE(pBitmap);
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawPicture(std::wstring strFileName, RECT decRect)
{
	auto ret = DrawPicture(CreateBitmap(const_cast<wchar_t *>(strFileName.c_str())), decRect);
	return ret;
}

bool My2DDraw::DrawPicture(std::wstring strFileName, RECT decRect, RECT srcRect)
{
	auto ret = DrawPicture(CreateBitmap(const_cast<wchar_t *>(strFileName.c_str()),0,0,&srcRect), decRect);
	return ret;
}

ID2D1Layer * My2DDraw::CreateLayer(ID2D1RenderTarget * thisRenderTarget)
{
	if (!thisRenderTarget) return nullptr;
	ID2D1Layer * mTempLayer = nullptr;
	auto hr = thisRenderTarget->CreateLayer(&mTempLayer);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "CreateLayer失败!");
	return mTempLayer;
}

void My2DDraw::PushLayer(ID2D1Layer * layer)
{
	mRenderTarget->PushLayer(D2D1::LayerParameters(
		D2D1::InfiniteRect(),
		NULL,
		D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(),
		1.0,
		NULL,
		D2D1_LAYER_OPTIONS_NONE),
		layer);
}

void My2DDraw::PopLayer(ID2D1Layer * layer)
{
	mRenderTarget->PopLayer();
}

bool My2DDraw::Clear(MyColor color)
{
	if (!mRenderTarget) return false;
	mRenderTarget->BeginDraw();
	mRenderTarget->Clear(color);
	return SUCCEEDED(mRenderTarget->EndDraw());
}

bool My2DDraw::Present(RECT  * pRect)
{
	IS_ERROR_EXIT(!mTempTarget,"mTempTarget为空");
	ID2D1Bitmap * tempBitmap = nullptr;
	auto hr = mTempTarget->GetBitmap(&tempBitmap);
	IS_ERROR_EXIT(FAILED(hr), "GetBitmap失败!");

	SetCurrentRenderTarget(mHwndRenderTarget);
	DrawPicture(tempBitmap,*pRect);
	SetCurrentRenderTarget(mTempTarget);

	SAFE_RELEASE(tempBitmap);
	return true;
}

My2DDraw::~My2DDraw()
{
	SAFE_RELEASE(mTempTarget);
	SAFE_RELEASE(mHwndRenderTarget);
	SAFE_RELEASE(mWriteFactory);
	SAFE_RELEASE(mFactory);
}
