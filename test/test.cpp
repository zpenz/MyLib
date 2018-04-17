#include "stdafx.h"

#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h>
#include "../MyLib/lib.h"
#include "../MyLib/2Draw.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#include "../MyLib/CommonDialog.h"
#pragma comment(lib,"../Debug/lib.lib")
#include <iostream>
using namespace std;

inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

int main()
{
	using namespace CommonItem;

	

	BaseWindow bs;
	auto ThreadHandle = bs.Show();

		Sleep(2000);

		_CrtDumpMemoryLeaks();
		POINT  * pPoint = new POINT();
		GetCursorPos(pPoint);
		COLORREF color = GetPixel(GetDC(NULL), pPoint->x, pPoint->y);
		cout << (UINT)GetRValue(color) << (UINT)GetGValue(color) << (UINT)GetBValue(color) << endl;

		auto liser = bs.mListener;
		auto drawable = DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125));
		//SAVE
		liser.AddClickFuncByID(120, [&]() {
			printf("开始保存...\n");
			auto path = DlgManager.ShowSaveFileDialog();

			Layout::ControlLayout.SaveLayoutFile("D:\\window.layout",DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mSaveSet);
			printf("保存完毕...\n");
		});

		//LabelBox
		liser.AddClickFuncByID(126, [&]() {
			drawable->mStateType = L"LabelBox";
			printf("选择LabelBox\n");
		});

		//Force
		liser.AddClickFuncByID(127, [&]() {
			drawable->mStateType = L"ForceLabel";
			printf("选择ForceLabel\n");
		});

		//EditBox
		liser.AddClickFuncByID(128, [&]() {
			drawable->mStateType = L"NoCaretEditBox";
			printf("选择NoCaretEditBox\n");
		});

		//ImgButton
		liser.AddClickFuncByID(129, [&]() {
			drawable->mStateType = L"ImageButton";
			printf("选择ImageButton\n");
		});

		//StaticLabel
		liser.AddClickFuncByID(130, [&]() {
			drawable->mStateType = L"StaticLabel";
			printf("选择StaticLabel\n");
		});

		//关闭
		liser.AddClickFuncByID(152, [&]() {
			drawable->mStateType = L"CloseButton";
			printf("选择CloseButton\n");
		});

		//最小
		liser.AddClickFuncByID(153, [&]() {
			drawable->mStateType = L"MiniButton";
			printf("选择MiniButton\n");
		});

		//最大
		liser.AddClickFuncByID(154, [&]() {
			drawable->mStateType = L"MaxButton";
			printf("选择MaxButton\n");
		});

		//DrawAbleLabel
		liser.AddClickFuncByID(125, [&]() {
			using namespace Conver;
			auto pControl = drawable->pForcedControl;
			if (pControl == nullptr)
			{
				liser.SetRangeIDValue(141, 151, L"null");
			}
			else
			{
				COLORREF tempColor = pControl->BackColor();
				liser.SetValueByID(142, wFormat(L"%d", pControl->width()));
				liser.SetValueByID(143, wFormat(L"%d", pControl->height()));
				liser.SetValueByID(145, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->ForceColor();
				liser.SetValueByID(146, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->HoverBackColor();
				liser.SetValueByID(147, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->HoverForceColor();
				liser.SetValueByID(148, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				liser.SetValueByID(149, wFormat(L"%f", pControl->getBoardSize()));
				tempColor = pControl->getBoardColor();
				liser.SetValueByID(150, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				liser.SetValueByID(151, wFormat(L"%d", pControl->CanDrag()));
				liser.SetValueByID(141, pControl->Text());
				liser.SetValueByID(157, wFormat(L"%d", pControl->HaveBoard()));
			}
		});

		//更新属性
		liser.AddClickFuncByID(155, [&]() {
			using namespace Layout;
			using namespace Conver;
			auto UpdateData = [&](Control * pControl) {
				//更新
				pControl->SetWidth(utoi(liser.getValueByID(142)));
				pControl->SetHeight(utoi(liser.getValueByID(143)));
				pControl->SetBackColor(ControlLayout.getColor(liser.getValueByID(145)));
				pControl->SetForceColor(ControlLayout.getColor(liser.getValueByID(146)));
				pControl->SetHoverBackColor(ControlLayout.getColor(liser.getValueByID(147)));
				pControl->SetHoverForceColor(ControlLayout.getColor(liser.getValueByID(148)));
				pControl->setBoardSize(utof(liser.getValueByID(149)));
				pControl->setBoardColor(ControlLayout.getColor(liser.getValueByID(150)));
				pControl->SetDrag(utob(liser.getValueByID(151)));
				pControl->SetText(liser.getValueByID(141));
				pControl->SetBoard(utob(liser.getValueByID(157)));		    
			};

			auto pControl = DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->pForcedControl;
			IS_RETURN(!pControl,);
			printf("开始更新...\n");
			auto id = pControl->getID();
			auto findControl = find_if(drawable->mSaveSet.begin(), drawable->mSaveSet.end(), [&id](Control * pControl) {
				if (pControl->getID() == id) return true;
				return false;
			});
			if (findControl == drawable->mSaveSet.end()) return;

			UpdateData(*findControl);
			UpdateData(pControl);
			
			drawable->SaveControl(1024.0f/drawable->width(),768.0f/drawable->height(),pControl);
			printf("更新完成...\n");
		});


		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
    return 0;
}

