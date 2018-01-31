/*
-------------------------------------------------
	libD3D(not complete)
	last devise : 2016��7��31�� 22:18:22
	purpose     :  more convince  to use DirectX9
--------------------------------------------------
*/
# include "stdafx.h"
# include "windows.h"
# include "Mmsystem.h"
# include "libD3D.h"


extern void ErrorMessage(const char * _error);

lib3D9::lib3D9():BaseWindow()
{
	this->mDirect9   = NULL;
	this->mDevice9 = NULL;
	
	SetWindowName("LIB DX9");
}

lib3D9::~lib3D9()
{
	if(this->mDirect9!=NULL)
	{
		mDirect9->Release();
	}

	if(this->mDevice9!=NULL)
	{
		mDevice9->Release();
	}
}

void lib3D9::AfterCreate(void)
{
	if(!this->InitD3D())
	{
		ErrorMessage("InitD3D Error!");
	}
	else
	{
		
	}
}

bool lib3D9::InitD3D(void)
{
	mDirect9 = Direct3DCreate9(D3D_SDK_VERSION);
	if(mDirect9==NULL)
	{
		ErrorMessage("IDirect3D9 ����ʧ��!");
		exit(0);
		return false;
	}
	D3DCAPS9 cap ;
	ZeroMemory(&cap,sizeof(D3DCAPS9));

	//����õ��豸
	mDirect9->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&cap);

	int vp = 0;
	if(cap.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		//֧�ֶ�������
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	//���D3DPRESENT_PARAMETERS
	D3DPRESENT_PARAMETERS ppm;
	ZeroMemory(&ppm,sizeof(ppm));
	ppm.BackBufferWidth =  this->GetWidth();				//��̨����Ŀ��
	ppm.BackBufferHeight = this->GetHeight();				//��̨����ĸ߶�
	ppm.BackBufferCount  = 1;								//��̨���������
	ppm.BackBufferFormat =	D3DFMT_A8R8G8B8	;				//��̨����ĸ�ʽ
	ppm.EnableAutoDepthStencil = true;						//�Ƿ���ģ�建�����Ȼ���
	ppm.AutoDepthStencilFormat = D3DFMT_D24S8;;				//�Զ�ģ�����Ȼ����ʽ
	ppm.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	ppm.hDeviceWindow          = (HWND)*this;
	ppm.MultiSampleQuality     = 0;
	ppm.MultiSampleType        = D3DMULTISAMPLE_NONE;
	ppm.Windowed               = true;
	ppm.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	ppm.Flags                  = 0;

	HRESULT ht = mDirect9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,(HWND)*this,vp,&ppm,&this->mDevice9);

	if(FAILED(ht))
	{
		
		ErrorMessage("�����豸ʧ�ܣ�");
		exit(0);
		return false;
	}

	return true;
}


void lib3D9::OnCreate()
{
	
}

void lib3D9::InitBeforeCreate()
{
	
}

void lib3D9::OnDraw()
{

}

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
			//��û����Ϣ��ʱ�� ִ�б�Ĳ���
			float current_time = (float)timeGetTime();
			float delta_time   = (current_time-last_time)*0.001f;
			this->Display(delta_time);

			//UpdateTime
			last_time = current_time;
				
		}
	}
}

void lib3D9::Display(float delta)
{
	mDevice9->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0x00000000,1.0f,0);
	mDevice9->Present(0,0,0,0);
}