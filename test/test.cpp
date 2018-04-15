#include "stdafx.h"

#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>

#include<crtdbg.h>

#include "../MyLib/lib.h"
#include "../MyLib/2Draw.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#pragma comment(lib,"../Debug/lib.lib")
#include <iostream>
using namespace std;



inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

int main()
{

	BaseWindow bs;
	auto ThreadHandle = bs.Show();

		Sleep(2000);

		_CrtDumpMemoryLeaks();
		POINT  * pPoint = new POINT();
		GetCursorPos(pPoint);
		COLORREF color = GetPixel(GetDC(NULL), pPoint->x, pPoint->y);
		cout << (UINT)GetRValue(color) << (UINT)GetGValue(color) << (UINT)GetBValue(color) << endl;


		//SAVE
		bs.mListener.AddClickFuncByID(120, [&]() {
			printf("开始保存...\n");
			Layout::ControlLayout.SaveLayoutFile("D:\\window.layout", &bs.mListener);
			printf("保存完毕...\n");
		});

		//LabelBox
		bs.mListener.AddClickFuncByID(126, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"LabelBox";
			printf("选择LabelBox\n");
		});

		//Force
		bs.mListener.AddClickFuncByID(127, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"ForceLabel";
			printf("选择ForceLabel\n");
		});

		//EditBox
		bs.mListener.AddClickFuncByID(128, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"NoCaretEditBox";
			printf("选择NoCaretEditBox\n");
		});

		//ImgButton
		bs.mListener.AddClickFuncByID(129, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"ImageButton";
			printf("选择ImageButton\n");
		});

		//StaticLabel
		bs.mListener.AddClickFuncByID(130, [&]() {
			DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mStateType = L"StaticLabel";
			printf("选择StaticLabel\n");
		});


		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);

    return 0;
}

