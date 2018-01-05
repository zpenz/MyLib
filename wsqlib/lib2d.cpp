#include "stdafx.h"
#include "lib2d.h"

D2D1_RECT_F & recto2(RECT & rc)
{
	D2D1_RECT_F df;
	df.bottom = rc.bottom;
	df.top = rc.top;
	df.left = rc.left;
	df.right = rc.right;
	return df;
}

rp::rp()
{
	this->left_pos = 0;
	this->need_height = 0;
	this->need_width = 0;
	this->pBitmap = NULL;
	this->top_pos = 0; 
	this->sw_height = 0;
	this->sw_width = 0;
	this->need_clip = false;
	this->is_high_render = false;
	//清空区域
	ZeroMemory(&this->sw_rc,sizeof(RECT));
	this->url = NULL;
}

rp::~rp()
{
	this->left_pos = 0;
	this->top_pos = 0;
	SAFE_Release(pBitmap);
	this->need_height = 0;
	this->need_width  = 0;
}

 
lib2d::lib2d() :My_Window()
{
	this->pBrush = NULL;
	this->pFactory = NULL;
	this->pRenderTarget = NULL;
	this->mwindowname = "wsqD2D";
	//清零 默认为黑色
	ZeroMemory(&brushcolor,sizeof(D2D1_COLOR_F));
	this->v_bitmap = new vector<Render_Bitmap>();

}

//添加BITMAP
bool lib2d::AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y)
{
	Render_Bitmap * pRenderBitmap = new Render_Bitmap();
	pRenderBitmap->url = pic_name;
	pRenderBitmap->need_width = des_width;
	pRenderBitmap->need_height = des_height;
	pRenderBitmap->top_pos = pos_y;
	pRenderBitmap->left_pos = pos_x;
	this->v_bitmap->push_back(*pRenderBitmap);
	//释放资源
	delete(pRenderBitmap);
	pRenderBitmap = NULL;
	return true;
}

//version2 :: 带有显示宽度和高度的版本
bool lib2d::AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y,int shw_width,int shw_height)
{
	Render_Bitmap * pRenderBitmap = new Render_Bitmap();
	pRenderBitmap->url = pic_name;
	pRenderBitmap->need_width = des_width;
	pRenderBitmap->need_height = des_height;
	pRenderBitmap->top_pos = pos_y;
	pRenderBitmap->left_pos = pos_x;
	pRenderBitmap->sw_width = shw_width;
	pRenderBitmap->sw_height = shw_height;
	this->v_bitmap->push_back(*pRenderBitmap);
	//释放资源
	delete(pRenderBitmap);
	pRenderBitmap = NULL;
	return true;
}

//version3:: 可以剪切版本
bool lib2d:: AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc)
{
	Render_Bitmap * pRenderBitmap = new Render_Bitmap();
	pRenderBitmap->url = pic_name;
	pRenderBitmap->need_width = des_width;
	pRenderBitmap->need_height = des_height;
	pRenderBitmap->top_pos = pos_y;
	pRenderBitmap->left_pos = pos_x;
	pRenderBitmap->sw_rc = sw_rc;
	pRenderBitmap->alpha = alpha;
	pRenderBitmap->is_high_render = is_high_render;
	//剪切为真
	pRenderBitmap->need_clip = true;
	this->v_bitmap->push_back(*pRenderBitmap);
	//释放资源
	delete(pRenderBitmap);
	pRenderBitmap = NULL;
	return true;
}

//version4:: 修正版本
bool lib2d::AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc,int sw_width,int sw_height)
{
	Render_Bitmap * pRenderBitmap = new Render_Bitmap();
	pRenderBitmap->url = pic_name;
	pRenderBitmap->need_width = des_width;
	pRenderBitmap->need_height = des_height;
	pRenderBitmap->top_pos = pos_y;
	pRenderBitmap->left_pos = pos_x;
	pRenderBitmap->sw_rc = sw_rc;
	pRenderBitmap->alpha = alpha;
	pRenderBitmap->is_high_render = is_high_render;
	pRenderBitmap->sw_width = sw_width;
	pRenderBitmap->sw_height = sw_height;
	//剪切为真
	pRenderBitmap->need_clip = true;
	this->v_bitmap->push_back(*pRenderBitmap);
	//释放资源
	delete(pRenderBitmap);
	pRenderBitmap = NULL;
	return true;
}

//version5:: finally version
bool lib2d::AddBitmap(wchar_t * pic_name,int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc, int sw_width, int sw_height)
{
	Render_Bitmap * pRenderBitmap = new Render_Bitmap();
	pRenderBitmap->url = pic_name;
	pRenderBitmap->top_pos = pos_y;
	pRenderBitmap->left_pos = pos_x;
	pRenderBitmap->sw_rc = sw_rc;
	pRenderBitmap->alpha = alpha;
	pRenderBitmap->is_high_render = is_high_render;
	pRenderBitmap->sw_width = sw_width;
	pRenderBitmap->sw_height = sw_height;
	//剪切为真
	pRenderBitmap->need_clip = true;
	this->v_bitmap->push_back(*pRenderBitmap);
	//释放资源
	delete(pRenderBitmap);
	pRenderBitmap = NULL;
	return true;
}

void lib2d::cleanup()
{
	SAFE_Release(this->pBrush);
	SAFE_Release(this->pFactory);
	SAFE_Release(this->pRenderTarget);
}

void lib2d::Destory()
{
	//释放资源
	cleanup();
}

void lib2d::Init()
{
	this->SetBrush((HBRUSH)GetStockObject(BLACK_BRUSH));
	//this->SetWindowStyle(WS_OVERLAPPEDWINDOW|~WS_BORDER);
	
}

void lib2d::OnCreate()
{
}

void lib2d::DrawRectangle()
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	this->pRenderTarget->DrawRectangle(D2D1::RectF(0, 0, 500, 500), this->pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}

void lib2d::DrawRectangle(RECT & rc)
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	this->pRenderTarget->DrawRectangle(D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom), this->pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}

void lib2d::DrawRectangle(int left,int top,int right,int bottom)
{
	this->InitResource();
	this->pRenderTarget->BeginDraw();
	this->pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	this->pRenderTarget->DrawRectangle(D2D1::RectF(left, top, right, bottom), this->pBrush, 1.0f);
	this->pRenderTarget->EndDraw();
}


void lib2d::OnDraw()
{
	HRESULT hs;
	Draw();
	this->InitResource();
	this->LoadBitmapResource();
	this->pRenderTarget->BeginDraw();
	this->Clear();
	this->DrawBitmap();
	hs = this->pRenderTarget->EndDraw();
	if (FAILED(hs))
	{
		Error_Box("Draw Error!");
	}
	
}

void lib2d::Clear()
{
}

void lib2d::Draw()
{
}

void lib2d::AfterCreate()
{
	if (!this->InitResource())
	{
		Error_Box("init Resource failed  need to close");
		exit(0);
	}
}

bool lib2d::SetBrushColor(int a,int r,int g,int b)
{
	this->brushcolor.a = a;
	this->brushcolor.r = r;
	this->brushcolor.g = g;
	this->brushcolor.b = b;
	return true;
}

bool lib2d::InitResource()
{
	HRESULT hs = NULL;
	hs = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->pFactory);
	if (FAILED(hs))
	{
		Error_Box("Init pFactory Failed！");
		return false;
	}

	HWND _hwnd = GetHwnd();
	if (_hwnd == NULL)
	{
		Error_Box("Get Hwnd Failed!");
		return false;
	}

	RECT rc;
	if (!GetClientRect(_hwnd, &rc))
	{
		Error_Box("Get Client Rect Failed!");
		return false;
	}

	hs = pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(_hwnd,
		D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),&pRenderTarget);

	if (!SUCCEEDED(hs))
	{
		Error_Box("init HwndRenderTarget failed!");
		return false;
	}
	
	//设置画刷颜色=-=
	this->SetBrushColor(this->brushcolor.a,this->brushcolor.r,this->brushcolor.g,this->brushcolor.b);
	//画刷
	hs = pRenderTarget->CreateSolidColorBrush(this->brushcolor, &pBrush);
	if (!SUCCEEDED(hs))
	{
		Error_Box("Init SolidBrushFailed");
		return false;
	}

	// Create WIC factory
	hs = CoCreateInstance(
		CLSID_WICImagingFactory1,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void **>(&pWICFactory)
		);

	if (FAILED(hs))
	{
		Error_Box("Load Bitmap Failed!");
		return false;
	}
	return true;
}

//
// Creates a Direct2D bitmap from the specified
// file name.
//
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
	IWICStream *pStream = NULL;
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
	SAFE_Release(pDecoder);
	SAFE_Release(pSource);
	SAFE_Release(pStream);
	SAFE_Release(pConverter);
	SAFE_Release(pScaler);
	return hr;
}

//擦除背景
void lib2d::ClearBackground(int a, int r, int g, int b)
{
	D2D1_COLOR_F cf;
	cf.a = a;
	cf.r = r;
	cf.g = g;
	cf.b = b;	
	this->pRenderTarget->Clear(cf);
}


void lib2d::DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width = 0, int des_height = 0)
{

	ID2D1Bitmap * pBp = NULL;
	ID2D1Bitmap * pBp2 = NULL;
	HRESULT hs; 
	//初始化资源
	this->InitResource();
	//LoadBitmap
	hs = LoadBitmapFromFile(
		pRenderTarget,
		pWICFactory,
		pic_name,
		des_width,
		des_height,
		&pBp);

	hs = LoadBitmapFromFile(
		pRenderTarget,
		pWICFactory,
		L"1.jpg",
		des_width,
		des_height,
		&pBp2);

	if (FAILED(hs))
	{
		Error_Box("Load Bitmap Failed!");
		return ;
	}

	this->pRenderTarget->BeginDraw();
	//白色背景
	//pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	D2D1_SIZE_F size = pBp->GetSize();

	pRenderTarget->DrawBitmap(
		pBp2,
		D2D1::RectF(pos_x-100, pos_y-100, pos_x + size.width,
		pos_y + size.height));

	//绘图
	pRenderTarget->DrawBitmap(
		pBp,
		D2D1::RectF(pos_x, pos_y, pos_x + size.width,
		pos_y + size.height));

	hs = this->pRenderTarget->EndDraw();
	if (FAILED(hs))
	{
		Error_Box("Draw Failed!");
		exit(0);
	}
}

//大改
void lib2d::DrawBitmap2(wchar_t * pic_name, int pos_x, int pos_y, int des_width = 0, int des_height = 0)
{
	//加入图形列表
	this->AddBitmap(pic_name,des_width,des_height,pos_x,pos_y);

	
}

//一次性加载所有的位图资源
void lib2d::LoadBitmapResource()
{
	vector<Render_Bitmap>::iterator it;
	for (it = v_bitmap->begin(); it != v_bitmap->end();it++)
	{
		if((*it).sw_height==0 && (*it).sw_width ==0)
		{
			this->LoadBitmapFromFile(pRenderTarget, pWICFactory, (*it).url, (*it).need_width, (*it).need_height, &(*it).pBitmap);
		}
		else // 显示参数
		{
			this->LoadBitmapFromFile(pRenderTarget, pWICFactory, (*it).url, (*it).sw_width, (*it).sw_height, &(*it).pBitmap);
		}
		
	}
}

//一次渲染所有位图
void lib2d::DrawBitmap()
{
	vector<Render_Bitmap>::iterator it;
	for(it = v_bitmap->begin(); it != v_bitmap->end(); it++)
	{
		//不需要剪切
		if (!(*it).need_clip)
		{
			this->pRenderTarget->DrawBitmap((*it).pBitmap,
				D2D1::RectF((*it).left_pos, (*it).top_pos, (*it).left_pos + (*it).need_width,
				(*it).top_pos + (*it).need_height));
	      
		} 
		//只需要显示一部分图形
		else
		{
			this->pRenderTarget->DrawBitmap((*it).pBitmap,
				D2D1::RectF((*it).left_pos, (*it).top_pos, (*it).left_pos + (*it).sw_width == 0 ? (*it).need_width : (*it).sw_width, (*it).top_pos + (*it).sw_height == 0 ? (*it).need_height : (*it).sw_height),
				(*it).alpha,
				(*it).is_high_render ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				recto2((*it).sw_rc));
		}
	}
}

lib2d::~lib2d()
{

}
