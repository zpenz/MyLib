#include "stdafx.h"
#include "lib2d.h"

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
		:mLeft(left),mRight(right),mTop(top),mBottom(bottom) 
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

bool My2DDraw::InitManager()
{
	mFactory = NULL;
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory);
	IS_RETURN_ERROR(FAILED(hr), false, "CreateFactory失败");
	return true;
}

bool My2DDraw::SetRenderTarget(HWND hTargetWindowHwnd, RECT * pRect)
{
	IS_RETURN_ERROR(!hTargetWindowHwnd,false,"hTargetWindowHwnd不存在");
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
	auto hr=mFactory->CreateHwndRenderTarget(
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
	ID2D1SolidColorBrush * pSolidBrush ;
	auto hr = mRenderTarget->CreateSolidColorBrush(penColor, &pSolidBrush);
	if (SUCCEEDED(hr)) return pSolidBrush;
	return NULL;
}

ID2D1Bitmap * My2DDraw::CreateBitmap(wchar_t * BitmapFileName,UINT dstWidth,UINT dstHeight)
{
	ID2D1Bitmap * pD2DBitmap= nullptr;
	CoInitialize(NULL);
	IWICImagingFactory * pImagingFactory = nullptr;
	auto hr = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pImagingFactory));
	IS_RETURN_ERROR(FAILED(hr), nullptr, "创建IWICImagingFactory接口Instance失败!");

	IWICBitmapDecoder * pDecoder=nullptr;
	hr = pImagingFactory->CreateDecoderFromFilename(
		BitmapFileName, 
		NULL,
		GENERIC_READ, //no need write
		WICDecodeMetadataCacheOnLoad,
		&pDecoder);
	IS_RETURN_ERROR(FAILED(hr),nullptr,"从图片文件创建解码器失败!");

	//get zero frame
	IWICBitmapFrameDecode * pSource = nullptr;
	hr = pDecoder->GetFrame(0,&pSource);
	IS_RETURN_ERROR(FAILED(hr), nullptr, "获取第一frame图片失败!");

	UINT oldSizeWidth, oldSizeHeight;
	pSource->GetSize(&oldSizeWidth,&oldSizeHeight);
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
		hr = pScaler->Initialize(pSource,dstWidth,dstHeight,WICBitmapInterpolationModeCubic);
		IS_RETURN_ERROR(FAILED(hr), nullptr, "初始化缩放Scaler失败!");

		hr = pFormatConverter->Initialize(pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}
	IS_RETURN_ERROR(FAILED(hr), nullptr, "转换图片失败!");

	hr = mRenderTarget->CreateBitmapFromWicBitmap(pFormatConverter,NULL, &pD2DBitmap);
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
	if(tempSolidBrush ==NULL)
		tempSolidBrush = CreateBrush();
	mRenderTarget->BeginDraw();
	D2D1_RECT_F desRect = MyRect(Rect);
	if(!isFillRectangle)
		mRenderTarget->DrawRectangle(desRect, tempSolidBrush);
	else
		mRenderTarget->FillRectangle(desRect, tempSolidBrush);
	auto hr = mRenderTarget->EndDraw();
	return SUCCEEDED(hr);
}

bool My2DDraw::DrawEllipse(POINT centerPoint, float r1, float r2, bool isFillEllipse, ID2D1SolidColorBrush * pSoildBrush)
{
	auto tempSolidBrush = pSoildBrush;
	if (tempSolidBrush == NULL)
		tempSolidBrush = CreateBrush();
	mRenderTarget->BeginDraw();
	if(!isFillEllipse)
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
		mRenderTarget->DrawLine(PointToD2DPointF(src), PointToD2DPointF(des),tempSolidBrush);
		auto hr = mRenderTarget->EndDraw();
	return SUCCEEDED(hr);
	return false;
}

bool My2DDraw::DrawPicture(ID2D1Bitmap * pBitmap, RECT decRect)
{
	IS_RETURN_ERROR(pBitmap==NULL,false,"位图结构为空");
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
