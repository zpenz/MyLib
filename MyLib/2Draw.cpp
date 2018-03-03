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

	auto tempRect = pRect;
	if (tempRect == NULL)
	{
		tempRect = new RECT();
		GetWindowRect(hTargetWindowHwnd, tempRect);
	}
	auto width = tempRect->right - tempRect->left;
	auto height = tempRect->bottom - tempRect->top;

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
	if (!mRenderTarget) return false;
	return true;
}

ID2D1SolidColorBrush * My2DDraw::CreateBrush(MyColor penColor)
{
	ID2D1SolidColorBrush * pSolidBrush;
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

	if (SUCCEEDED(hr)) return pD2DBitmap;

	return nullptr;
}

ID2D1RenderTarget * My2DDraw::getRenderTarget()
{
	return mRenderTarget; 
}

bool My2DDraw::DrawRectangle(RECT Rect, MyColor RectColor, bool isFillRectangle)
{
	IS_RETURN_ERROR(mRenderTarget==nullptr,false,"RenderTarget为空");
	auto tempSolidBrush = CreateBrush(RectColor);

	mRenderTarget->BeginDraw();
	D2D1_RECT_F desRect = MyRect(Rect);
	if (!isFillRectangle)
		mRenderTarget->DrawRectangle(desRect, tempSolidBrush);
	else
		mRenderTarget->FillRectangle(desRect, tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();
	auto error = GetLastError();
	return SUCCEEDED(hr);
}

IDWriteTextLayout * My2DDraw::CreateTextLayout(std::string text)
{
	return CreateTextLayout(text,18.0f);
}

IDWriteTextLayout * My2DDraw::CreateTextLayout(std::string text, float fSize)
{
	if (text.empty()) return nullptr;
	IDWriteTextFormat * tempTextFormat = NULL;
	auto hr = mWriteFactory->CreateTextFormat(L"新宋体", NULL, DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSize, L"en-us", &tempTextFormat);

	IDWriteTextLayout * tempTextLayout = NULL;
	hr = mWriteFactory->CreateTextLayout(Conver::ACharToWChar(const_cast<char *>(text.c_str())), text.length(), tempTextFormat, 100.0f, 0.0f,
		&tempTextLayout);
	IS_ERROR_EXIT(FAILED(hr), "创建TextLayout失败!");
	return tempTextLayout;
}

bool My2DDraw::DrawRectWithText(RECT Rect, std::string text,MyColor RectColor, MyColor TextColor, UINT AlignType,bool isFillRectangle)
{
	if(!DrawRectangle(Rect,RectColor, isFillRectangle)) return false;
	if (text.empty()) return true;

	IDWriteTextLayout * tempTextLayout = CreateTextLayout(text);

	///<LayOutBox>MaxWidth-MaxHeight</LayOutBox>
	tempTextLayout->SetMaxWidth(static_cast<FLOAT>(RECTWIDTH(Rect)));
	tempTextLayout->SetMaxHeight(static_cast<FLOAT>(RECTHEIGHT(Rect)));

	if (AlignType & ALIGN_LEFT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
	if (AlignType & ALIGN_RIGHT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	}
	if (AlignType & ALIGN_RIGHT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); 
	}
	if (AlignType & ALIGN_TOP)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	if (AlignType & ALIGN_BOTTOM)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}
	if(AlignType & ALIGN_CENTER_H)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); 
	}
	if(AlignType & ALIGN_CENTER_V)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); 
	}
	if(AlignType == ALIGN_DEFAULT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);  //水平居中
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); //垂直居中
	}
	
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(Rect)),tempTextLayout,CreateBrush(TextColor),D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);
	auto ret = mRenderTarget->EndDraw();
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
	return SUCCEEDED(hr);
	return false;
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
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect)
{
	IS_ERROR_EXIT(!mRenderTarget,"mRenderTarget为空");
	IS_ERROR_EXIT(!pBitmap, "位图结构为空");
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawBitmap(pBitmap, RectToD2DRectF(decRect));
	auto hr = mRenderTarget->EndDraw();
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
	IS_RETURN_ERROR(!mTempTarget,false,"mTempTarget为空");
	ID2D1Bitmap * tempBitmap = nullptr;
	mTempTarget->GetBitmap(&tempBitmap);

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
