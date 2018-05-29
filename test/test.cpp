#include "stdafx.h"

//#define _CRTDBG_MAP_ALLOC 
#include<stdlib.h>
#include<crtdbg.h>
#include "../MyLib/lib.h"
#include "../MyLib/2Draw.h"
#include "../MyLib/Macros.h"
#include "../MyLib/Control.h"
#include "../MyLib/CommonDialog.h"
#ifdef _DEBUG
#pragma comment(lib,"../Debug/lib.lib")
#else
#pragma comment(lib,"../Release/lib.lib")
#endif
#include <iostream>
using namespace std;

int CALLBACK WinMain(HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	int nShowCmd)
{
	using namespace CommonItem;
	using namespace MutexLock;
	 
	BaseWindow bs("");
	bs.UseWrapper();
	auto ThreadHandle = bs.Show();

		Sleep(2000);

		auto liser = bs.mListener;
		auto drawable = DYCAST(DrawAbleLabel *, liser.findElementByID(125));

		auto CheckExist = [&](UINT id) {
		};

		auto Log = [&](wstring info) {
			//״̬..
			IS_RETURN(!liser.findElementByID(123),false);
			liser.findElementByID(123)->SetText(info);
			return true;
		};

#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
#endif

		POINT  * pPoint = new POINT();
		GetCursorPos(pPoint);
		COLORREF color = GetPixel(GetDC(NULL), pPoint->x, pPoint->y);
		cout << (UINT)GetRValue(color) << (UINT)GetGValue(color) << (UINT)GetBValue(color) << endl;

		//SAVE
		liser.AddClickFuncByID(120, [&]() {
			Log(L"��ʼ����...");
			auto path = DlgManager.ShowSaveFileDialog();
			if (path.empty()) return;
			if (path.find(L".layout") == wstring::npos) path = path + L".layout";
			Layout::ControlLayout.SaveLayoutFile(Conver::WCharToAChar(path), drawable->mSaveSet);
			Log(L"�������...");
		});

		//Load
		liser.AddClickFuncByID(158, [&]() {
			printf("������...\n");
			auto path = DlgManager.ShowOpenFileDialog();
			bs.Close();
			CloseHandle(ThreadHandle);   

			BaseWindow bsNew(Conver::WCharToAChar(path));
			auto newHandle = bsNew.Show();
			ThreadHandle = newHandle;

			WaitForSingleObject(newHandle,INFINITE);
			printf("�������...\n");
		});

		Log(L"װ�غ���...");

		//LabelBox
		liser.AddClickFuncByID(126, [&]() {
			drawable->mStateType = L"LabelBox";
			Log(L"ѡ��LabelBox");
		});

		//Force
		liser.AddClickFuncByID(127, [&]() {
			drawable->mStateType = L"ForceLabel";
			Log(L"ѡ��ForceLabel");
		});

		//EditBox
		liser.AddClickFuncByID(128, [&]() {
			drawable->mStateType = L"EditBox";
			Log(L"ѡ��EditBox");
		});

		//ImgButton
		liser.AddClickFuncByID(129, [&]() {
			drawable->mStateType = L"ImageButton";
			Log(L"ѡ��ImageButton");
		});

		//StaticLabel
		liser.AddClickFuncByID(130, [&]() {
			drawable->mStateType = L"StaticLabel";
			Log(L"ѡ��StaticLabel");
		});

		//�ر�
		liser.AddClickFuncByID(152, [&]() {
			drawable->mStateType = L"CloseButton";
			Log(L"ѡ��CloseButton");
		});

		//��С
		liser.AddClickFuncByID(153, [&]() {
			drawable->mStateType = L"MiniButton";
			Log(L"ѡ��MiniButton");
		});

		//���
		liser.AddClickFuncByID(154, [&]() {
			drawable->mStateType = L"MaxButton";
			Log(L"ѡ��MaxButton");
		});

		//TitleBar
		liser.AddClickFuncByID(159, [&]() {
			drawable->mStateType = L"TitleBar";
			Log(L"ѡ��TitleBar");
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
				liser.SetValueByID(142, Format(L"%d", pControl->width()));
				liser.SetValueByID(143, Format(L"%d", pControl->height()));
				liser.SetValueByID(145, Format(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->ForceColor();
				liser.SetValueByID(146, Format(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->HoverBackColor();
				liser.SetValueByID(147, Format(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				tempColor = pControl->HoverForceColor();
				liser.SetValueByID(148, Format(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				liser.SetValueByID(149, Format(L"%f", pControl->getBoardSize()));
				tempColor = pControl->getBoardColor();
				liser.SetValueByID(150, Format(L"%d %d %d", (UINT)GetRValue(tempColor), (UINT)GetGValue(tempColor), (UINT)GetBValue(tempColor)));
				liser.SetValueByID(151, Format(L"%d", pControl->CanDrag()));
				liser.SetValueByID(141, pControl->Text());
				liser.SetValueByID(157, Format(L"%d", pControl->HaveBoard()));
			}
		});

		//��������
		liser.AddClickFuncByID(155, [&]() {
			using namespace Layout;
			using namespace Conver;
			auto UpdateData = [&](Control * pControl) {
				//����
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
			printf("��ʼ����...\n");
			auto id = pControl->getID();
			auto findControl = find_if(drawable->mSaveSet.begin(), drawable->mSaveSet.end(), [&id](Control * pControl) {
				if (pControl->getID() == id) return true;
				return false;
			});
			if (findControl == drawable->mSaveSet.end()) return;

			UpdateData(*findControl);
			UpdateData(pControl);
			
			drawable->SaveControl(1024.0f/drawable->width(),768.0f/drawable->height(),pControl);
			printf("�������...\n");
		});
		Log(L"����");

		WaitForSingleObject(ThreadHandle, INFINITE);
		CloseHandle(ThreadHandle);
    return 0;
}

