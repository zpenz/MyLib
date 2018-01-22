#pragma once
#include "stdafx.h"
#ifdef INCLUDE_MY_DX11
#include "libd11.h"

#define D11_DEBUG 111
#ifdef D11_DEBUG
#include <iostream>
using namespace std;
#endif

BufferManager * BufferManager::pInstance = NULL;
BufferManager * BufferManager::getInstance()
{
	if (!pInstance) 
		pInstance = new BufferManager();
	return pInstance;
}

void libd11::Init()
{
	this->SetWindowName("Mini DirectX11 program");
}

void libd11::ClearBackColor(D3DXCOLOR  cl)
{
	background_color = cl;
}

void libd11::SetTargetRender()
{
	ID3D11Texture2D * pTexture2D;
	pswapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pTexture2D);
	pdev->CreateRenderTargetView(pTexture2D,NULL,&pRenderTargetView);
	Safe_Release(pTexture2D);
	pcon->OMSetRenderTargets(1,&pRenderTargetView,NULL);
}

void libd11::CleanUp()
{
	for (int i = 0; i < this->v_buffers.size(); i++)
	{
		Safe_Release(this->v_buffers[i].pIbuffer);
		Safe_Release(this->v_buffers[i].pVbuffer);
	}
	Safe_Release(pLayout);
	Safe_Release(pdev);
	Safe_Release(pcon);
	Safe_Release(pswapchain);
	Safe_Release(pRenderTargetView);
	Safe_Release(ps);
	Safe_Release(vs);
}

void libd11::AfterCreate()
{
	DXGI_SWAP_CHAIN_DESC dsc;
	ZeroMemory(&dsc, sizeof(dsc));
	dsc.BufferCount = 1;
	dsc.BufferDesc.Height = GetHeight();
	dsc.BufferDesc.Width = GetWidth();
	dsc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dsc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dsc.SampleDesc.Count = 4;
	dsc.Windowed = true;
	dsc.OutputWindow = GetHwnd();

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&dsc,
		&pswapchain,
		&pdev,
		NULL,
		&pcon
		);

	SetTargetRender();
	SetViewPort();
	initpiple();
	initGhrips();//init data
}

void libd11::initGhrips()
{
	for (int i = 0; i < v_buffers.size(); i++)
	{
		//vertex buffer
		D3D11_BUFFER_DESC bd, id;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		bd.ByteWidth = sizeof(Vertex) * v_buffers[i].vcount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ID3D11Buffer * pVBuffer = NULL;
		pdev->CreateBuffer(&bd, NULL, &pVBuffer);
		D3D11_MAPPED_SUBRESOURCE ms;
		pcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
		memcpy(ms.pData, v_buffers[i].vx_array, sizeof(Vertex));                 // copy the data
		pcon->Unmap(pVBuffer, NULL);


		//index buffer
		ZeroMemory(&id, sizeof(id));
		id.Usage = D3D11_USAGE_DEFAULT;
		id.ByteWidth = sizeof(UINT) * v_buffers[i].icount;
		id.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ID3D11Buffer * pIBuffer = NULL;
		D3D11_SUBRESOURCE_DATA ibdata;
		ZeroMemory(&ibdata, sizeof(ibdata));
		ibdata.pSysMem = v_buffers[i].id_array;
		pdev->CreateBuffer(&id, &ibdata, &pIBuffer);

		v_buffers[i].pIbuffer = pIBuffer;
		v_buffers[i].pVbuffer = pVBuffer;
	}
}

bool libd11::InitD11()
{
	AfterCreate();
	if (pdev == NULL || pcon == NULL || pswapchain == NULL)
	{ 
		Error_Box("初始化错误！终止程序");		exit(-1); return false;
	}
	return true;
}

void libd11::myDraw()
{

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (int i = 0; i < v_buffers.size(); i++)
	{
#ifdef D11_DEBUG
		if (v_buffers[i].pVbuffer == NULL || v_buffers[i].pIbuffer==NULL) printf("Error_\n");
		cout << v_buffers[i].pVbuffer << v_buffers[i].pIbuffer << v_buffers[i].icount << endl;
#endif
		pcon->IASetVertexBuffers(0, 1, &v_buffers[i].pVbuffer, &stride, &offset);
		pcon->IASetIndexBuffer(v_buffers[i].pIbuffer, DXGI_FORMAT_R32_UINT, 0);
		pcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pcon->Draw(v_buffers[i].vcount,0);
		pcon->DrawIndexed(v_buffers[i].icount,0,0);
		
	}
}

void libd11::OwnOperator(float delata)
{
	pcon->ClearRenderTargetView(pRenderTargetView, background_color);
	this->myDraw();
	pswapchain->Present(0, 0);
}

void libd11::LoadVertexAndIndex(Vertex * pv, int VertexCount, UINT * index, int IndexCount)
{
	buffer_desc desc;
	desc.pVbuffer = NULL;
	desc.pIbuffer = NULL;
	desc.vx_array = pv;
	desc.id_array = index;
	desc.icount = IndexCount;
	desc.vcount = VertexCount;
	v_buffers.push_back(desc);
}

void libd11::SetShaderName(UINT shader_kind, const char * shader_file_name)
{
	//validate
	FILE * pfile = fopen(shader_file_name,"r");
	if (!pfile)
	{
		Error_Box("can not load shader file please check if the file name is true");
		exit(-1);
	}
		
	switch (shader_kind)
	{
	case SR_PIXEL:
		ps_name = shader_file_name;
		break;
	case SR_VERTEX:
		vs_name = shader_file_name;
		break;
	default:
		break;
	}
}

void libd11::initpiple()
{
	if (!vs_name || !ps_name)
	{
		Error_Box("没有载入shader文件 请至少载入vertex shader 和 pixel shader");
		exit(-1);
	}
		ID3D10Blob *VS, *PS;
		D3DX11CompileFromFile(vs_name, 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
		D3DX11CompileFromFile(ps_name, 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

		// encapsulate both shaders into shader objects
		pdev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &vs);
		pdev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &ps);

		// set the shader objects
		pcon->VSSetShader(vs, 0, 0);
		pcon->PSSetShader(ps, 0, 0);

		// create the input layout object
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		pdev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
		pcon->IASetInputLayout(pLayout);
}

void libd11::SetViewPort()
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = GetWidth();
	viewport.Height = GetHeight();
	pcon->RSSetViewports(1, &viewport);
}


libd11::~libd11()
{
	CleanUp();
}
#endif