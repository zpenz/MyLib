#pragma once
#ifdef USE_3D
#ifdef SONGAPI
#define SONGAPI _declspec(dllimport)
#else 
#define SONGAPI _declspec(dllexport)
#endif 

#include "../MyLib/lib.h" 
#pragma comment(lib,"../Debug/lib.lib")

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib") 

#define SCREEN_WIDTH   1920
#define SCREEN_HEIGHT  1080

extern "C"
{
	struct SONGAPI VERTEX
	{
		//position
		FLOAT X, Y, Z;
		//color
		D3DXCOLOR Color;
	};

	SONGAPI  bool  DoSomething();

	class SONGAPI MyGraphics
	{
		SINGLE_INSTANCE(MyGraphics);

	private:
		IDXGISwapChain * mSwapChain;		

		ID3D11Device * mDevice;                    

		ID3D11DeviceContext * mDeviceContext;        

		ID3D11RenderTargetView * mpRenderTargetView;      

		ID3D11InputLayout * mpLayout;					

		ID3D11VertexShader * mpVertexShader;          

		ID3D11PixelShader * mpPixelShader;        

		ID3D11Buffer * mpVertexBuffer;          

		ID3D11Buffer * mpIndexBuffer;                       

	public:
		bool InitD3D(HWND hWnd);

		bool InitGraphics();

		bool InitPipeline();

		void SetViewPort();

		void CleanUp();

		void Draw();
		
	};

#define GraphicsManager MyGraphics::getInstance()
}

#endif