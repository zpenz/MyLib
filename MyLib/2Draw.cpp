#include "2Draw.h"
#include <DWrite.h>
# include <wincodec.h>
#include <atlbase.h>

using namespace Conver;

//conver 
D2D1_POINT_2F & PointToD2DPointF(POINT & pt)
{
	D2D1_POINT_2F *pPointF = new D2D1_POINT_2F(
		D2D1::Point2F(static_cast<float>(pt.x),
			static_cast<float>(pt.y)));
	return *pPointF;
}

D2D1_RECT_F & RectToD2DRectF(RECT & rc)
{
	D2D1_RECT_F * pNeedRect = new D2D1_RECT_F(
		D2D1::RectF(static_cast<float>(rc.left),
			static_cast<float>(rc.top),
			static_cast<float>(rc.right),
			static_cast<float>(rc.bottom)));
	return *pNeedRect;
}

class MyRect
{
private:
	RECT mRect;
	int mLeft;
	int mRight;
	int mTop;
	int mBottom;
public:
	MyRect(INT left, INT top, INT right, INT bottom)
		:mLeft(left), mRight(right), mTop(top), mBottom(bottom)
	{
		mRect = { mLeft,mTop,mRight,mBottom };
	}

	MyRect(RECT desRect)
		:mRect(desRect) {}

	operator RECT&()
	{
		return mRect;
	}

	operator D2D1_RECT_F&()
	{
		return RectToD2DRectF(*this);
	}
};


bool My2DDraw::InitManager()
{
	mFactory = NULL;
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateFactory失败");
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),(IUnknown **)&mWriteFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateWriteFactory失败");
	return true;
}

bool My2DDraw::SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect)
{
	IS_RETURN_ERROR(!hTargetWindowHwnd, false, "hTargetWindowHwnd不存在");
	auto tempRect = pRect;
	if (tempRect == NULL)
	{
		tempRect = new RECT();
		GetClientRect(hTargetWindowHwnd, tempRect);
	}
	auto width = tempRect->right - tempRect->left;
	auto height = tempRect->bottom - tempRect->top;
	mRenderTarget = NULL;
	auto tp = D2D1::RenderTargetProperties();
	tp.dpiX = 96;
	tp.dpiY = 96;
	auto hr = mFactory->CreateHwndRenderTarget(
		tp,
		D2D1::HwndRenderTargetProperties(
			hTargetWindowHwnd,
			D2D1::SizeU((int)width, (int)height)
		), &mRenderTarget);
	if (FAILED(hr)) return false;
	mRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	return true;
}

ID2D1SolidColorBrush * My2DDraw::CreateBrush(D2D1::ColorF penColor)
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

bool My2DDraw::DrawRectangle(RECT Rect, bool isFillRectangle, ID2D1SolidColorBrush * pSolidBrush)
{
	//create brush
	auto tempSolidBrush = pSolidBrush;
	if (tempSolidBrush == NULL)
		tempSolidBrush = CreateBrush();
	mRenderTarget->BeginDraw();
	D2D1_RECT_F desRect = MyRect(Rect);
	if (!isFillRectangle)
		mRenderTarget->DrawRectangle(desRect, tempSolidBrush);
	else
		mRenderTarget->FillRectangle(desRect, tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();
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

bool My2DDraw::DrawRectWithText(RECT Rect, std::string text, bool isFillRectangle, ID2D1SolidColorBrush * pSoildBrush)
{
	if(!DrawRectangle(Rect,isFillRectangle,pSoildBrush)) return false;
	IDWriteTextLayout * tempTextLayout = CreateTextLayout(text);
	float fMaxWidth; 
	auto hr = tempTextLayout->DetermineMinWidth(&fMaxWidth);
	IS_ERROR_EXIT(FAILED(hr),"计算最大宽度错误！");

	///<LayOutBox>MaxWidth-MaxHeight</LayOutBox>
	tempTextLayout->SetMaxWidth(static_cast<FLOAT>(RECTWIDTH(Rect)));
	tempTextLayout->SetMaxHeight(static_cast<FLOAT>(RECTHEIGHT(Rect)));
	///<水平居中对齐>////
	tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	///<垂直居中对齐>////
	tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(Rect)),tempTextLayout,CreateBrush(MyColor::White));
	mRenderTarget->EndDraw();
	return true;
}

bool My2DDraw::DrawEllipse(POINT centerPoint, float r1, float r2, bool isFillEllipse, ID2D1SolidColorBrush * pSoildBrush)
{
	auto tempSolidBrush = pSoildBrush;
	if (tempSolidBrush == NULL)
		tempSolidBrush = CreateBrush();
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
	return false;
}

bool My2DDraw::DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect)
{
	IS_RETURN_ERROR(pBitmap == NULL, false, "位图结构为空");
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawBitmap(pBitmap, RectToD2DRectF(decRect));
	auto hr = mRenderTarget->EndDraw();
	return false;
}



My2DDraw::~My2DDraw()
{
	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mFactory);
}
