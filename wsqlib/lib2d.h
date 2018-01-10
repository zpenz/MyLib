#pragma once
# include "lib.h"
# include <d2d1.h>
# include <wincodec.h>
# include <vector>
using namespace std;
# pragma comment(lib,"d2d1.lib")
# pragma comment(lib,"windowscodecs.lib")
# pragma warning(disable:4244)
# pragma warning(disable:4172)
//ת��
D2D1_RECT_F & recto2(RECT & rc);
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
	//����
	ID2D1Factory * pFactory;
	//��ȾĿ��
	ID2D1HwndRenderTarget * pRenderTarget;
	//��ˢ
	ID2D1SolidColorBrush       * pBrush;
	//λͼ
	ID2D1Bitmap         * pBitmap;
	IWICImagingFactory *  pWICFactory;
	//ʹ��vector ά��һϵ�е�λͼ
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
	//��ͼ����д������
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

