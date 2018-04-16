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
			//Layout::ControlLayout.SaveLayoutFile("D:\\window.layout", &bs.mListener);
			Layout::ControlLayout.SaveLayoutFile("D:\\window.layout",DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->StretchSetRect(1024, 768));
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

		//DrawAbleLabel
		bs.mListener.AddClickFuncByID(125, [&]() {
			using namespace Conver;
			auto pControl = DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->pForcedControl;
			if (pControl == nullptr)
			{
				bs.mListener.SetRangeIDValue(141, 151, L"null");
			}
			else
			{
				COLORREF tempColor = pControl->BackColor();
				bs.mListener.SetValueByID(142, wFormat(L"%d", pControl->width()));
				bs.mListener.SetValueByID(143, wFormat(L"%d", pControl->height()));
				bs.mListener.SetValueByID(145, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->ForceColor();
				bs.mListener.SetValueByID(146, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->HoverBackColor();
				bs.mListener.SetValueByID(147, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->HoverForceColor();
				bs.mListener.SetValueByID(148, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				bs.mListener.SetValueByID(149, wFormat(L"%f", pControl->getBoardSize()));
				tempColor = pControl->getBoardColor();
				bs.mListener.SetValueByID(150, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				bs.mListener.SetValueByID(151, wFormat(L"%d", pControl->CanDrag()));
				bs.mListener.SetValueByID(141, pControl->Text());
			}
		});


		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
    return 0;
}

