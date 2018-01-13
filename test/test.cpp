#include "stdafx.h"
#include "../MyLib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 
	DrawManager.InitManager();
	auto hw = FindWindow(L"Notepad", L"�½��ı��ĵ�.txt - ���±�");
	RECT rc,rcClient;
	GetClientRect(hw, &rcClient);
	if (!DrawManager.SetRenderTarget(hw)) return 0;
	rc.top = 0;
	rc.left = 0;
	rc.right = 100;
	rc.bottom = 100;
	auto pBitmap = DrawManager.CreateBitmap(L"1.jpg",100,100);
	DrawManager.DrawPicture(pBitmap,rc);
	DrawManager.DrawRectangle(rc,DrawManager.CreateBrush(MyColor::Pink));
    return 0;
}

