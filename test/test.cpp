// test.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "../wsqlib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 

	DrawManager.InitManager();
	DrawManager.SetRenderTarget(FindWindow(L"Notepad",L"�½��ı��ĵ�.txt - ���±�"));
	RECT rc;
	rc.bottom = 100;
	rc.left = 0;
	rc.right = 100;
	rc.bottom = 100;
	DrawManager.DrawRectangle(rc);
    return 0;
}

