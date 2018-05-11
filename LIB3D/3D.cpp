#include "3D.h"

class TheWindow :public BaseWindow
{
public:

	void AfterCreate() override
	{
		GraphicsManager.InitD3D(GetHwnd());
	}

	void Update(float delta) override
	{
		GraphicsManager.Draw();
	}

	void OnDraw() override
	{

	}

	void  Destory() override
	{
		GraphicsManager.CleanUp();
	}

};

VERTEX OurVertices[] =
{
	{ -0.5f, 0.5f, 0.0f,  D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },  //左上  0
	{ 0.5f, -0.5, 0.0f,   D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },  //右下  1
	{ -0.5f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },  //左下  2
	{ 0.5f, 0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f) }     //右上  3
};

bool DoSomething()
{
	TheWindow win;
	auto ThreadID = win.Show();
	WaitForSingleObject(ThreadID, INFINITE);
	return true;
}

bool MyGraphics::InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext);

	//init RenderTargetView
	ID3D11Texture2D * pBackBuffer;
	//getbuffer
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
	// 使用back buffer地址 创建RenderTarget
	mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mpRenderTargetView);
	//release
	pBackBuffer->Release();
	//setRenderTargets
	mDeviceContext->OMSetRenderTargets(1, &mpRenderTargetView, NULL);

	//setviewport
	SetViewPort();

	InitPipeline();
	InitGraphics();

	return true;
}

bool MyGraphics::InitGraphics()
{
	// create a triangle using the VERTEX struct
	HRESULT result;

	UINT index[] =
	{
		0,1,2,
		0,3,1
	};

	//create the index buffer
	D3D11_BUFFER_DESC ib;
	ZeroMemory(&ib, sizeof(ib));

	ib.Usage = D3D11_USAGE_DEFAULT;
	ib.ByteWidth = sizeof(UINT) * 6;
	ib.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibdata;
	ZeroMemory(&ibdata, sizeof(ibdata));
	ibdata.pSysMem = index;
	result = mDevice->CreateBuffer(&ib, &ibdata, &mpIndexBuffer);
	IS_ERROR_EXIT(FAILED(result), "创建indexBuffer失败");

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 4;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	mDevice->CreateBuffer(&bd, NULL, &mpVertexBuffer);

	D3D11_MAPPED_SUBRESOURCE ms;
	mDeviceContext->Map(mpVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    // map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
	mDeviceContext->Unmap(mpVertexBuffer, NULL);
	return true;
}

bool MyGraphics::InitPipeline()
{
	// load and compile the two shaders
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate both shaders into shader objects
	mDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &mpVertexShader);
	mDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &mpPixelShader);

	// set the shader objects
	mDeviceContext->VSSetShader(mpVertexShader, 0, 0);
	mDeviceContext->PSSetShader(mpPixelShader, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mDevice->CreateInputLayout(inputElementDesc, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &mpLayout);
	mDeviceContext->IASetInputLayout(mpLayout);
	
	return true;
}

void MyGraphics::SetViewPort()
{
	D3D11_VIEWPORT viewPort;
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = SCREEN_WIDTH;
	viewPort.Height = SCREEN_HEIGHT;

	mDeviceContext->RSSetViewports(1, &viewPort);
}


void MyGraphics::CleanUp()
{
	SAFE_RELEASE(mpLayout);
	SAFE_RELEASE(mpVertexShader);
	SAFE_RELEASE(mpPixelShader);
	SAFE_RELEASE(mpVertexBuffer);
	SAFE_RELEASE(mpIndexBuffer);
	SAFE_RELEASE(mSwapChain);
	SAFE_RELEASE(mpRenderTargetView);
	SAFE_RELEASE(mDevice);
	SAFE_RELEASE(mDeviceContext);
}

void MyGraphics::Draw()
{
	// clear the back buffer to a deep blue
	mDeviceContext->ClearRenderTargetView(mpRenderTargetView, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// select which primtive type we are using
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->DrawIndexed(6, 0, 0);
	// switch the back buffer and the front buffer
	mSwapChain->Present(0, 0);
}

