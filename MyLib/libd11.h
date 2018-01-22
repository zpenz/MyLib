#pragma once
#ifdef INCLUDE_MY_DX11
#include "lib.h"
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <vector>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define Safe_Release(s) if(!s) s->Release();
#define SR_PIXEL 250
#define SR_VERTEX 251

using namespace std;

typedef struct
{
	FLOAT x, y, z;
	D3DXCOLOR color;
}Vertex;

typedef struct
{
	ID3D11Buffer * pIbuffer;
	ID3D11Buffer * pVbuffer;
	Vertex * vx_array;
	UINT   * id_array;
	UINT icount;
	UINT vcount;
}buffer_desc;

class BufferManager
{
private:
	BufferManager() {};
	static BufferManager * pInstance;
public:
	static BufferManager * getInstance();
	~BufferManager() { delete pInstance; }
};

class libd11 :public My_Window
{

private:
	ID3D11PixelShader * ps;
	ID3D11VertexShader * vs;
	vector<buffer_desc> v_buffers;
	ID3D11Device * pdev;
	ID3D11DeviceContext * pcon;
	IDXGISwapChain * pswapchain;
	ID3D11InputLayout * pLayout;
	ID3D11Buffer * pBackBuffer;
	ID3D11RenderTargetView * pRenderTargetView;
	D3DXCOLOR  background_color;
	const char * ps_name;
	const char * vs_name;
	virtual void AfterCreate();
	virtual void OwnOperator(float delata);
	void initGhrips();
	void initpiple();
	void SetTargetRender();
	void Init();
public:
	bool InitD11();
	void CleanUp();
	virtual void myDraw();
	void SetShaderName(UINT shader_kind, const char * shader_file_name);
	void LoadVertexAndIndex(Vertex * vx, int VertexCount, UINT * index, int IndexCount);
	void SetViewPort();
	void ClearBackColor(D3DXCOLOR color);
	libd11() :pdev(NULL), pcon(NULL), background_color(D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)) {}
	~libd11();
};

#endif