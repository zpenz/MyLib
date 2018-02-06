#include "2Draw.h"
#include <DWrite.h>
# include <wincodec.h>
#include <atlbase.h>

using namespace Conver;

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

ID2D1Bitmap * My2DDraw::CreateBitmap(wchar_t * BitmapFileName, UINT dstWidth, UINT dstHeight)
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

	//get zero frame
	IWICBitmapFrameDecode * pSource = nullptr;
	hr = pDecoder->GetFrame(0, &pSource);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "获取第一frame图片失败!");

	UINT oldSizeWidth, oldSizeHeight;
	pSource->GetSize(&oldSizeWidth, &oldSizeHeight);
	//scale
	IWICFormatConverter * pFormatConverter = nullptr;
	hr = pImagingFactory->CreateFormatConverter(&pFormatConverter);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "创建格式转换器失败");

	IWICBitmapScaler * pScaler = nullptr;
	if (dstHeight == 0 && dstWidth == 0)
	{
		hr = pFormatConverter->Initialize(pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}
	else
	{
		//
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
		//
		hr = pImagingFactory->CreateBitmapScaler(&pScaler);
		IS_RETURN_ERROR(FAILED(hr), nullptr, "创建缩放Scaler失败!");
		hr = pScaler->Initialize(pSource, dstWidth, dstHeight, WICBitmapInterpolationModeCubic);
		IS_RETURN_ERROR(FAILED(hr), nullptr, "初始化缩放Scaler失败!");

		hr = pFormatConverter->Initialize(pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}
	IS_RETURN_ERROR(FAILED(hr), nullptr, "转换图片失败!");

	hr = mRenderTarget->CreateBitmapFromWicBitmap(pFormatConverter, NULL, &pD2DBitmap);
	if (SUCCEEDED(hr))
	{
		SAFE_RELEASE(pScaler);
		SAFE_RELEASE(pFormatConverter);
		SAFE_RELEASE(pSource);
		SAFE_RELEASE(pDecoder);
		SAFE_RELEASE(pImagingFactory);
		CoUninitialize();
		return pD2DBitmap;
	}
	return nullptr;
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
	IDWriteTextFormat * tempTextFormat = NULL;
	auto hr = mWriteFactory->CreateTextFormat(L"新宋体",NULL,DWRITE_FONT_WEIGHT_REGULAR,
	DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,18.0f,L"en-us",&tempTextFormat);

	IDWriteTextLayout * tempTextLayout = NULL;
	USES_CONVERSION;
	hr = mWriteFactory->CreateTextLayout(A2W(text.c_str()),text.length(), tempTextFormat,100,0,
		&tempTextLayout);
	IS_ERROR_EXIT(FAILED(hr),"创建TextLayout失败!");
	return tempTextLayout;
}

bool My2DDraw::DrawRectWithText(RECT Rect, std::string text,MyColor RectColor, MyColor TextColor, UINT AlignType,bool isFillRectangle)
{
	if(!DrawRectangle(Rect,RectColor, isFillRectangle)) return false;
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
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(Rect)),tempTextLayout,CreateBrush(TextColor), D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);
	mRenderTarget->EndDraw();
	return true;
}

bool My2DDraw::DrawRectWithPicture(RECT Rect, MyColor RectColor, std::wstring strFileName, UINT uOffSetX , UINT uOffSetY, bool isFillRectangle)
{
	if (!DrawRectangle(Rect, RectColor, isFillRectangle)) return false;
	OffsetRect(&Rect,uOffSetX,uOffSetY);
	auto ret = DrawPicture(CreateBitmap(const_cast<wchar_t *>(strFileName.c_str())), Rect);
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

bool My2DDraw::DrawLine(POINT src, POINT des, ID2D1SolidColorBrush * pSoildBrush)
{
	auto tempSolidBrush = pSoildBrush;
	if (tempSolidBrush == NULL)
		tempSolidBrush = CreateBrush();
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawLine(PointToD2DPointF(src), PointToD2DPointF(des), tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect)
{
	IS_RETURN_ERROR(pBitmap == NULL, false, "位图结构为空");
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawBitmap(pBitmap, RectToD2DRectF(decRect));
	auto hr = mRenderTarget->EndDraw();
	return SUCCEEDED(hr);
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
	if (!mTempTarget) return false;
	ID2D1Bitmap * tempBitmap;
	mTempTarget->GetBitmap(&tempBitmap);

	SetCurrentRenderTarget(mHwndRenderTarget);
	DrawPicture(tempBitmap,*pRect);
	SetCurrentRenderTarget(mTempTarget);

	SAFE_RELEASE(tempBitmap);
	return true;
}

My2DDraw::~My2DDraw()
{
	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mWriteFactory);
	SAFE_RELEASE(mFactory);
}
