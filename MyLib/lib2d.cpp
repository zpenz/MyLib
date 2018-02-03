#include "stdafx.h"
#include "lib2d.h"
#include "Macros.h"
using namespace Conver;

PicStruct::~PicStruct()
{
	SAFE_RELEASE(mPicture);
}
 
lib2d::lib2d():pBrush(NULL),pFactory(NULL),pRenderTarget(NULL)
{
	SetWindowName("MyLib2D");
	//清零 默认为黑色
	ZeroMemory(&brushcolor,sizeof(D2D1_COLOR_F));
	this->pPictureSet = new vector<RenderPicture>();
}

//添加一张图进去
bool lib2d::AddBitmap(wchar_t * pic_name, RECT desRect)
{
	RenderPicture  RenderBitmap(pic_name,desRect);
	this->pPictureSet->push_back(RenderBitmap);
	return true;
}

bool lib2d::AddBitmap(wchar_t * pic_name, RECT desRect,int shw_width,int shw_height)
{
	RenderPicture  RenderBitmap(pic_name,desRect);
	RenderBitmap.mWidth = static_cast<float>(shw_width);
	RenderBitmap.mHeight =static_cast<float>(shw_height);
	this->pPictureSet->push_back(RenderBitmap);
	return true;
}

bool lib2d:: AddBitmap(wchar_t * pic_name,RECT desRect,float alpha, bool is_high_render, RECT sw_rc)
{
	RenderPicture RenderBitmap(pic_name, desRect);
	RenderBitmap.mPicRect = sw_rc;
	RenderBitmap.mPicAlpha = alpha;
	RenderBitmap.mIsHighRender = is_high_render;
	//剪切为真
	RenderBitmap.mIsNeedClip = true;
	this->pPictureSet->push_back(RenderBitmap);
	return true;
}

bool lib2d::AddBitmap(wchar_t * pic_name, RECT desRect, float alpha, bool is_high_render, RECT sw_rc,int sw_width,int sw_height)
{
	RenderPicture RenderBitmap(pic_name, desRect);
	RenderBitmap.mPicRect = sw_rc;
	RenderBitmap.mPicAlpha = alpha;
	RenderBitmap.mIsHighRender = is_high_render;
	//剪切为真
	RenderBitmap.mIsNeedClip = true;
	this->pPictureSet->push_back(RenderBitmap);
	return true;
}

void lib2d::cleanup()
{
	SAFE_RELEASE(this->pBrush);
	SAFE_RELEASE(this->pFactory);
	SAFE_RELEASE(this->pRenderTarget);
}

void lib2d::Destory()
{
	//释放资源
	cleanup();
}

void lib2d::DrawRectangle()
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	this->pRenderTarget->DrawRectangle(D2D1::RectF(0, 0, 500, 500), pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}

void lib2d::DrawRectangle(RECT & rc)
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	this->pRenderTarget->DrawRectangle(MyRect(rc.left, rc.top, rc.right, rc.bottom), pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}

void lib2d::DrawRectangle(int left,int top,int right,int bottom)
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	this->pRenderTarget->DrawRectangle(MyRect(left, top, right, bottom), pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}


void lib2d::OnDraw()
{
	Draw();
	InitResource();
	SetBitmapResource();
	pRenderTarget->BeginDraw();
	DrawBitmap();
	auto hr = pRenderTarget->EndDraw();
	IS_RETURN_ERROR(FAILED(hr),,"A Draw Problem has occur!");
}


void lib2d::Draw()
{
}

void lib2d::AfterCreate()
{
	if (!this->InitResource())
	{
		ErrorMessage("init Resource failed  need to close");
		exit(0);
	}
}

bool lib2d::SetBrushColor(float a, float r, float g, float b)
{
	brushcolor = {a,r,g,b};
	return true;
}

bool lib2d::InitResource()
{
	HRESULT hs = NULL;
	hs = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->pFactory);
	if (FAILED(hs))
	{
		ErrorMessage("Init pFactory Failed！");
		return false;
	}

	HWND _hwnd = GetHwnd();
	if (_hwnd == NULL)
	{
		ErrorMessage("Get Hwnd Failed!");
		return false;
	}

	RECT rc;
	if (!GetWindowRect(_hwnd, &rc))
	{
		ErrorMessage("Get Client Rect Failed!");
		return false;
	}

	hs = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(_hwnd,
		D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),&pRenderTarget);

	if (!SUCCEEDED(hs))
	{
		ErrorMessage("init HwndRenderTarget failed!");
		return false;
	}
	
	//设置画刷颜色=-=
	this->SetBrushColor(this->brushcolor.a,this->brushcolor.r,this->brushcolor.g,this->brushcolor.b);
	//画刷
	hs = pRenderTarget->CreateSolidColorBrush(this->brushcolor, &pBrush);
	if (!SUCCEEDED(hs))
	{
		ErrorMessage("Init SolidBrushFailed");
		return false;
	}

	hs = CoCreateInstance(
		CLSID_WICImagingFactory1,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory)
		);

	if (FAILED(hs))
	{
		ErrorMessage("Load Bitmap Failed!");
		return false;
	}
	return true;
}

// Creates a Direct2D bitmap from the specified file name.
HRESULT lib2d::LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR uri,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
	)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}
	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);
	return hr;
}

void lib2d::ClearBackground(float a, float r, float g, float b)
{
	D2D1_COLOR_F cf = {a,r,g,b};
	this->pRenderTarget->Clear(cf);
}

void lib2d::DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width = 0, int des_height = 0)
{
	this->AddBitmap(pic_name,MyRect(des_width,des_height,pos_x,pos_y));
}

void lib2d::SetBitmapResource()
{
	vector<RenderPicture>::iterator it;
	for (it = pPictureSet->begin(); it != pPictureSet->end();it++)
	{
		if((*it).mHeight==0 && (*it).mWidth ==0)
		{
			this->LoadBitmapFromFile(pRenderTarget, pWICFactory, (*it).mFileName, (*it).GetWidth(), (*it).GetHeight(), &(*it).mPicture);
		}
		else // 显示参数
		{
			this->LoadBitmapFromFile(pRenderTarget, pWICFactory, (*it).mFileName, 
				static_cast<int>((*it).mWidth),  //short cast
				static_cast<int>((*it).mHeight), //short cast
				&(*it).mPicture);
		}
		
	}
}

void lib2d::DrawBitmap()
{
	vector<RenderPicture>::iterator it;
	for(it = pPictureSet->begin(); it != pPictureSet->end(); it++)
	{
		//不需要剪切
		if (!(*it).mIsNeedClip)
		{
			this->pRenderTarget->DrawBitmap((*it).mPicture,RectToD2DRectF(
			(*it).mDesRect));
		} 
		//只需要显示一部分图形
		else
		{
			this->pRenderTarget->DrawBitmap((*it).mPicture,
				(D2D1_RECT_F)MyRect((*it).mDesRect),
				(*it).mPicAlpha,
				(*it).mIsHighRender ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				RectToD2DRectF((*it).mPicRect));
		}
	}
}

lib2d::~lib2d() {}

