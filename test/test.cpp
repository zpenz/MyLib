#include "stdafx.h"
#include "../MyLib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")


int main()
{ 
	DrawManager.InitManager();
	auto hw = FindWindow(L"Notepad", L"新建文本文档.txt - 记事本");
	RECT rc,rcClient;
	GetClientRect(hw, &rcClient);
	rc.top = 40+rcClient.top;
	rc.left = 40+rcClient.left;
	rc.right = rcClient.right-40;
	rc.bottom = rcClient.bottom-40;
	if(!DrawManager.SetRenderTarget(hw,&rc)) return 0;
	auto bs = DrawManager.CreateBrush(MyColor::Pink);
	auto pBitmap = DrawManager.CreateBitmap(L"2.png");
	DrawManager.DrawPicture(pBitmap,rc,rc);

    return 0;
}

