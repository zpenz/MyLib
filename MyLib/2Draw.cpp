#include "2Draw.h"
#include <DWrite.h>
# include <wincodec.h>
#include <atlbase.h>

using namespace Conver;

bool My2DDraw::InitManager()
{
	mFactory = NULL;
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &mFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateFactoryʧ��");
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),(IUnknown **)&mWriteFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateWriteFactoryʧ��");
	return true;
}

bool My2DDraw::SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect)
{
	IS_RETURN_ERROR(!hTargetWindowHwnd, false, "hTargetWindowHwnd������");

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

	ID2D1HwndRenderTarget *pTempRenderTarget;
	auto tp = D2D1::RenderTargetProperties();
	auto hr = mFactory->CreateHwndRenderTarget(
		tp,
		D2D1::HwndRenderTargetProperties(
			hTargetWindowHwnd,
			D2D1::SizeU((int)width, (int)height)
		),&pTempRenderTarget);

	IS_RETURN_ERROR(FAILED(hr), false, "����HWNDRenderTarget����!");

	///<warning>short-cut!</warning>
	mRenderTarget = pTempRenderTarget;
	

	if (FAILED(hr)) return false;
	mRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	return true;
}

bool My2DDraw::SetRenderTarget(HDC hDC,RECT * pRc)
{
	IS_RETURN_ERROR(!mFactory, false, "mFactory null");
	if (mRenderTarget != nullptr) SAFE_RELEASE(mRenderTarget); //clean

	ID2D1DCRenderTarget * pTempDcRenderTarget;
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);
	auto hr = mFactory->CreateDCRenderTarget(&props,&pTempDcRenderTarget);
	IS_RETURN_ERROR(FAILED(hr), false, "����DcRenderTarget����!");
	
	hr = pTempDcRenderTarget->BindDC(hDC, pRc);
	IS_RETURN_ERROR(FAILED(hr), false, "��ָ����DC��RECTʧ��!");

	///<warning>short-cut!</warning>
	mRenderTarget = pTempDcRenderTarget;
	IS_RETURN_ERROR(mRenderTarget == nullptr, false, "ת������");

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
	IS_RETURN_ERROR(FAILED(hr), nullptr, "����IWICImagingFactory�ӿ�Instanceʧ��!");

	IWICBitmapDecoder * pDecoder = nullptr;
	hr = pImagingFactory->CreateDecoderFromFilename(
		BitmapFileName,
		NULL,
		GENERIC_READ, //no need write
		WICDecodeMetadataCacheOnLoad,
		&pDecoder);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "��ͼƬ�ļ�����������ʧ��!");

	//get zero frame
	IWICBitmapFrameDecode * pSource = nullptr;
	hr = pDecoder->GetFrame(0, &pSource);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "��ȡ��һframeͼƬʧ��!");

	UINT oldSizeWidth, oldSizeHeight;
	pSource->GetSize(&oldSizeWidth, &oldSizeHeight);
	//scale
	IWICFormatConverter * pFormatConverter = nullptr;
	hr = pImagingFactory->CreateFormatConverter(&pFormatConverter);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "������ʽת����ʧ��");

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
		IS_RETURN_ERROR(FAILED(hr), nullptr, "��������Scalerʧ��!");
		hr = pScaler->Initialize(pSource, dstWidth, dstHeight, WICBitmapInterpolationModeCubic);
		IS_RETURN_ERROR(FAILED(hr), nullptr, "��ʼ������Scalerʧ��!");

		hr = pFormatConverter->Initialize(pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}
	IS_RETURN_ERROR(FAILED(hr), nullptr, "ת��ͼƬʧ��!");

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
	IS_RETURN_ERROR(mRenderTarget==nullptr,false,"RenderTargetΪ��");
	//create brush
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
	auto hr = mWriteFactory->CreateTextFormat(L"������",NULL,DWRITE_FONT_WEIGHT_REGULAR,
	DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,18.0f,L"en-us",&tempTextFormat);

	IDWriteTextLayout * tempTextLayout = NULL;
	USES_CONVERSION;
	hr = mWriteFactory->CreateTextLayout(A2W(text.c_str()),text.length(), tempTextFormat,100,0,
		&tempTextLayout);
	IS_ERROR_EXIT(FAILED(hr),"����TextLayoutʧ��!");
	return tempTextLayout;
}

bool My2DDraw::DrawRectWithText(RECT Rect, std::string text,MyColor RectColor, MyColor TextColor, UINT AlignType,bool isFillRectangle)
{
	if(!DrawRectangle(Rect,RectColor, isFillRectangle)) return false;
	IDWriteTextLayout * tempTextLayout = CreateTextLayout(text);
	float fMaxWidth; 
	auto hr = tempTextLayout->DetermineMinWidth(&fMaxWidth);
	IS_ERROR_EXIT(FAILED(hr),"���������ȴ���");

	///<LayOutBox>MaxWidth-MaxHeight</LayOutBox>
	tempTextLayout->SetMaxWidth(static_cast<FLOAT>(RECTWIDTH(Rect)));
	tempTextLayout->SetMaxHeight(static_cast<FLOAT>(RECTHEIGHT(Rect)));

	if (AlignType & ALIGN_LEFT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
	}
	else if (AlignType & ALIGN_RIGHT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	}
	else if (AlignType & ALIGN_RIGHT)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING); 
	}
	else if (AlignType & ALIGN_TOP)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	else if (AlignType & ALIGN_BOTTOM)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}
	else if(AlignType & ALIGN_CENTER_H)
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); 
	}
	else if(AlignType & ALIGN_CENTER_V)
	{
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); 
	}
	else
	{
		tempTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);  //ˮƽ����
		tempTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); //��ֱ����
	}
	
	mRenderTarget->BeginDraw();
	mRenderTarget->DrawTextLayout(PointToD2DPointF(LeftTopPoint(Rect)),tempTextLayout,CreateBrush(TextColor));
	mRenderTarget->EndDraw();
	return true;
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
	IS_RETURN_ERROR(pBitmap == NULL, false, "λͼ�ṹΪ��");
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

My2DDraw::~My2DDraw()
{
	SAFE_RELEASE(mRenderTarget);
	SAFE_RELEASE(mWriteFactory);
	SAFE_RELEASE(mFactory);
}