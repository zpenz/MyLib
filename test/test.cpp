#include "stdafx.h"
#include "../MyLib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")


int main()
{ 
	DrawManager.InitManager();
	auto hw = FindWindow(L"MSPaintView", L"");
	RECT rc,rcClient;
	GetClientRect(hw, &rcClient);
	rc.top = 40+rcClient.top;
	rc.left = 40+rcClient.left;
	rc.right = rcClient.right-40;
	rc.bottom = rcClient.bottom-40;
	DrawManager.SetRenderTarget(hw,&rc);
	auto bs = DrawManager.CreateBrush(MyColor::Pink);
	while(1)
	DrawManager.DrawRectangle(rc,bs);
    return 0;
}

