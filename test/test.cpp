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

		auto liser = bs.mListener;
		auto drawable = DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125));
		//SAVE
		liser.AddClickFuncByID(120, [&]() {
			printf("��ʼ����...\n");
			//Layout::ControlLayout.SaveLayoutFile("D:\\window.layout", &bs.mListener);
			Layout::ControlLayout.SaveLayoutFile("D:\\window.layout",DYCAST(DrawAbleLabel *, bs.mListener.findElementByID(125))->mSaveSet);
			printf("�������...\n");
		});

		//LabelBox
		liser.AddClickFuncByID(126, [&]() {
			drawable->mStateType = L"LabelBox";
			printf("ѡ��LabelBox\n");
		});

		//Force
		liser.AddClickFuncByID(127, [&]() {
			drawable->mStateType = L"ForceLabel";
			printf("ѡ��ForceLabel\n");
		});

		//EditBox
		liser.AddClickFuncByID(128, [&]() {
			drawable->mStateType = L"NoCaretEditBox";
			printf("ѡ��NoCaretEditBox\n");
		});

		//ImgButton
		liser.AddClickFuncByID(129, [&]() {
			drawable->mStateType = L"ImageButton";
			printf("ѡ��ImageButton\n");
		});

		//StaticLabel
		liser.AddClickFuncByID(130, [&]() {
			drawable->mStateType = L"StaticLabel";
			printf("ѡ��StaticLabel\n");
		});

		//�ر�
		liser.AddClickFuncByID(152, [&]() {
			drawable->mStateType = L"CloseButton";
			printf("ѡ��CloseButton\n");
		});

		//��С
		liser.AddClickFuncByID(153, [&]() {
			drawable->mStateType = L"MiniButton";
			printf("ѡ��MiniButton\n");
		});

		//���
		liser.AddClickFuncByID(154, [&]() {
			drawable->mStateType = L"MaxButton";
			printf("ѡ��MaxButton\n");
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
				liser.SetValueByID(145, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->ForceColor();
				liser.SetValueByID(146, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->HoverBackColor();
				liser.SetValueByID(147, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				tempColor = pControl->HoverForceColor();
				liser.SetValueByID(148, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				liser.SetValueByID(149, wFormat(L"%f", pControl->getBoardSize()));
				tempColor = pControl->getBoardColor();
				liser.SetValueByID(150, wFormat(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor), (UINT)GetRValue(tempColor)));
				liser.SetValueByID(151, wFormat(L"%d", pControl->CanDrag()));
				liser.SetValueByID(141, pControl->Text());
			}
		});

		//��������
		liser.AddClickFuncByID(155, [&]() {
			using namespace Conver;
			auto pControl = drawable->pForcedControl;
			IS_RETURN(!pControl,);
			printf("��ʼ����...\n");
			auto id = pControl->getID();
			auto findControl = find_if(drawable->mSaveSet.begin(), drawable->mSaveSet.end(), [&id](Control * pControl) {
				if (pControl->getID() == id) return true;
				return false;
			});
			if (findControl == drawable->mSaveSet.end()) return;

			//����
			(*findControl)->SetWidth(utoi(liser.getValueByID(142)));
			(*findControl)->SetWidth(utoi(liser.getValueByID(142)));
			printf("�������...\n");
		});


		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
    return 0;
}

