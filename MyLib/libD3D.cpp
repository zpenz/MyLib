/*
-----------------------------------
	wsqlib(nc)
	last devise : 2016年7月31日 22:18:22
	purpose     : Create a more convince interface for using DirectX9
-----------------------------------
*/
# include "stdafx.h"
# include "windows.h"
# include "Mmsystem.h"
# include "libD3D.h"


/*
=============
void Error_Box(const char * _error)
=============
*/
extern void Error_Box(const char * _error);

/*
==================
construct
==================
*/
lib3D9::lib3D9():BaseWindow()
{
	this->_d3d   = NULL;
	this->Device = NULL;
	
	SetWindowName("WSQ_3D");
	this->SetBrush((HBRUSH)GetStockObject(BLACK_BRUSH));
}

lib3D9::~lib3D9()
{
	if(this->_d3d!=NULL)
	{
		_d3d->Release();
	}

	if(this->Device!=NULL)
	{
		Device->Release();
	}
}

/*
================
AfterCreate 
================
*/
void lib3D9::AfterCreate(void)
{
	if(!this->InitD3D())
	{
		Error_Box("InitD3D Error!");
	}
	else
	{
		
	}
}

/*
==================
InitD3D
==================
*/
bool lib3D9::InitD3D(void)
{
	_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(_d3d==NULL)
	{
		Error_Box("IDirect3D9 创建失败!");
		exit(0);
		return false;
	}
	D3DCAPS9 cap ;
	ZeroMemory(&cap,sizeof(D3DCAPS9));

	//下面得到设备
	_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&cap);

	int vp = 0;
	if(cap.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		//支持顶点运算
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//填充D3DPRESENT_PARAMETERS
	D3DPRESENT_PARAMETERS ppm;
	ZeroMemory(&ppm,sizeof(ppm));
	ppm.BackBufferWidth =  this->GetWidth();				//后台缓冲的宽度
	ppm.BackBufferHeight = this->GetHeight();				//后台缓冲的高度
	ppm.BackBufferCount  = 1;								//后台缓冲的数量
	ppm.BackBufferFormat =	D3DFMT_A8R8G8B8	;				//后台缓冲的格式
	ppm.EnableAutoDepthStencil = true;						//是否开启模板缓存和深度缓存
	ppm.AutoDepthStencilFormat = D3DFMT_D24S8;;				//自动模板和深度缓存格式
	ppm.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	ppm.hDeviceWindow          = (HWND)*this;
	ppm.MultiSampleQuality     = 0;
	ppm.MultiSampleType        = D3DMULTISAMPLE_NONE;
	ppm.Windowed               = true;
	ppm.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	ppm.Flags                  = 0;

	HRESULT ht = _d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,(HWND)*this,vp,&ppm,&this->Device);

	if(FAILED(ht))
	{
		
		Error_Box("创建设备失败！");
		exit(0);
		return false;
	}

	return true;
}


/*
===============
virtual function  OnCreate
===============
*/
void lib3D9::OnCreate()
{
	
}

/*
===============
virtual function Init
===============
*/
void lib3D9::Init()
{
	
}

/*
===============
OnDraw()
===============
*/
void lib3D9::OnDraw()
{

}


/*
=============
MessageLoop
=============
*/
void lib3D9::MessageLoop()
{
	MSG message;
	int fmessage = 0;
	ZeroMemory(&message,sizeof(message));

	static float last_time = (float)timeGetTime();
	PeekMessage(&message,0,0,0,PM_NOREMOVE);

	while(message.message!=WM_QUIT)
	{
		int fmessage = PeekMessage(&message,0,0,0,PM_REMOVE);
		if(fmessage)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			//当没有消息的时候 执行别的操作
			float current_time = (float)timeGetTime();
			float delta_time   = (current_time-last_time)*0.001f;
			this->Display(delta_time);

			//UpdateTime
			last_time = current_time;
				
		}
	}
}

/*
==================
Display
绘制操作
==================
*/
void lib3D9::Display(float delta)
{
	this->Device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
	Device->Present(0,0,0,0);
}