//新建一个txt文档打开 用来测试
#include "stdafx.h"
#include "../MyLib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 
	DrawManager.InitManager();
	auto hw = FindWindow(L"Notepad", L"新建文本文档.txt - 记事本");
	RECT  rcClient;
	GetClientRect(hw, &rcClient);
	if (!DrawManager.SetRenderTarget(hw)) return 0;

	//bitmap
	RECT rcPicture = { 72,128,72*2,128*2 };
	auto pBitmap = DrawManager.CreateBitmap(L"3.jpg",72,128);
	DrawManager.DrawPicture(pBitmap, rcPicture);
	RECT rcRectangle = { 0,0,72,128 };
	DrawManager.DrawPicture(pBitmap, rcRectangle);

	//Rectangle
	rcRectangle = {150,150,200,200};
	auto pPinkBrush = DrawManager.CreateBrush(MyColor::Pink);
	DrawManager.DrawRectangle(rcRectangle, true,pPinkBrush);
	rcRectangle = {400,150,450,200};
	DrawManager.DrawRectangle(rcRectangle, true, pPinkBrush);

	//Ellipse
	POINT pt = {300,300};
	auto pGoldBrush = DrawManager.CreateBrush(MyColor::Gold);
	DrawManager.DrawEllipse(pt,100.0,50.0,false,pGoldBrush);
	pt = { 250,300 };
	DrawManager.DrawEllipse(pt, 10, 20, false, pGoldBrush);
	pt = { 340,300 };
	DrawManager.DrawEllipse(pt, 10, 20, false, pGoldBrush);
    return 0;
}

