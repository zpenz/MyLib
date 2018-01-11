#pragma once
# include "lib.h"
# include <d2d1.h>
# include <wincodec.h>
# include <vector>
#include <iostream>
using namespace std;
# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"windowscodecs.lib")
# pragma warning(disable:4244)
# pragma warning(disable:4172)
//ת��
D2D1_RECT_F & RectToD2DRectF(RECT & rc);

class My2DDraw
{
	SINGLE_INSTANCE(My2DDraw);
private:
	ID2D1Factory * mFactory;
	ID2D1HwndRenderTarget * mRenderTarget;
public:
	int a;
	void test() { std::cout << a<<std::endl; }
	bool InitManager();
	bool SetRenderTarget(_In_ HWND hTargetWindowHwnd,RECT * pRect=NULL);
	ID2D1SolidColorBrush * CreateBrush(D2D1::ColorF BrushColor = D2D1::ColorF::Black);
	bool DrawRectangle(_In_ RECT Rect, ID2D1SolidColorBrush * pSoildBrush = NULL);
};

#define DrawManager My2DDraw::getInstance() 
#define MyColor D2D1::ColorF

//λͼ��Ⱦ�ṹ
typedef struct rp
{
	//ͼƬ����
	wchar_t * url;
	//����Ҫλͼ���,�߶�
	//�����Ϊ0 ��ʹ��ԭͼ
	float need_width;
	float need_height;
	//���Ͻ�λ��
	float left_pos;
	float top_pos; 
	//��ʾ��Ⱥ͸߶�
	float sw_width;
	float sw_height;
	//��Ⱦ���Բ�ֵ���ǽ��ƴ���
	bool is_high_render;
	//��ԭͼѡ��һ������Ⱦ
	RECT sw_rc;
	//�Ƿ���Ҫ����
	bool need_clip;
	//��͸����
	float alpha;
	//λͼָ��
	ID2D1Bitmap * pBitmap;
	rp();
	~rp();

}Render_Bitmap;

class lib2d : public My_Window
{
private:
	ID2D1Factory * pFactory;
	ID2D1HwndRenderTarget * pRenderTarget;
	ID2D1SolidColorBrush       * pBrush;
	ID2D1Bitmap         * pBitmap;
	IWICImagingFactory *  pWICFactory;
	vector<Render_Bitmap> * v_bitmap;
	//�ͷ���Դ
	void cleanup();
	D2D1_COLOR_F brushcolor;
	//����λͼ��Դ
	void LoadBitmapResource();
public:
	
	//���λͼ
	bool AddBitmap(wchar_t * pic_name,int des_width, int des_height,int pos_x,int pos_y);
	bool AddBitmap(wchar_t * pic_name,int des_width,int des_height,int pos_x,int pos_y,int shw_width,int shw_height);
	//AddBitmap������ͼƬ���� ��� �߶� ��Ⱦλ�� ��͸���� �Ƿ��������Ⱦ ��ʾԭͼ���ĸ����������
	bool AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y,float alpha,bool is_high_render,RECT sw_rc);
	//AddBitmap������ͼƬ���� ��� �߶� ��Ⱦλ�� ��͸���� �Ƿ��������Ⱦ ��ʾԭͼ���ĸ���������� ��ʾ��� ��ʾ�߶�
	bool AddBitmap(wchar_t * pic_name, int des_width, int des_height, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc,int sw_width,int sw_height);
	//AddBitmap������ͼƬ���� ��� �߶� ��Ⱦλ�� ��͸���� �Ƿ��������Ⱦ ��ʾԭͼ���ĸ���������� ��ʾ��� ��ʾ�߶�(des_width��des_heightû��)
	bool AddBitmap(wchar_t * pic_name, int pos_x, int pos_y, float alpha, bool is_high_render, RECT sw_rc, int sw_width, int sw_height);

	virtual void Destory();
	virtual void OnCreate();
	virtual void Init();
	virtual void OnDraw() ;
	virtual void Draw();
	virtual void AfterCreate();
	virtual void  Clear();
	
	//��ʼ����Դ
	virtual bool InitResource();
	//���û�ˢ��ɫ
	bool SetBrushColor(int a,int r,int b,int g);
	//�������
	virtual void ClearBackground(int a,int r,int g,int b);
	virtual void DrawRectangle();
	virtual void DrawRectangle(RECT & rc);
	virtual void DrawRectangle(int left,int top,int right,int bottom);
	virtual void DrawBitmap();
	virtual void DrawBitmap(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);
	virtual void DrawBitmap2(wchar_t * pic_name, int pos_x, int pos_y, int des_width, int des_height);

	virtual HRESULT LoadBitmapFromFile(
		ID2D1RenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR uri,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
		);
	lib2d();
	~lib2d();
};

