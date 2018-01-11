// test.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "../wsqlib/lib2d.h"
#pragma comment(lib,"../Debug/lib.lib")

int main()
{ 

	DrawManager.InitManager();
	DrawManager.SetRenderTarget(FindWindow(L"Notepad",L"新建文本文档.txt - 记事本"));
	RECT rc;
	rc.bottom = 100;
	rc.left = 0;
	rc.right = 100;
	rc.bottom = 100;
	DrawManager.DrawRectangle(rc);
    return 0;
}

