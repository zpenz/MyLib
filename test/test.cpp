// test.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "../wsqlib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 
	DrawManager.InitManager();
	RECT rc;
	rc.top = 0;
	rc.left = 0;
	rc.right = 100;
	rc.bottom = 100;
	DrawManager.SetRenderTarget(FindWindow(L"Notepad",L"�½��ı��ĵ�.txt - ���±�"),&rc);
	auto bs = DrawManager.CreateBrush(MyColor::Red);
	while(1)
	DrawManager.DrawRectangle(rc,bs);
    return 0;
}

