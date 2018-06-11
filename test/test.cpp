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
	 
	BaseWindow baseWindow("");
	baseWindow.UseWrapper();
	auto ThreadHandle = baseWindow.Show();

	Sleep(2000);

	auto drawable = DYCAST(DrawAbleLabel *, baseWindow.findElementByID(125));

	auto Log = [&](wstring info) {
		//状态..
		IS_RETURN(!baseWindow.findElementByID(123),false);
		baseWindow.findElementByID(123)->SetText(info);
		Sleep(200);
		return true;
	};

	//SAVE
	baseWindow.AddClickFuncByID(120, [&]() {
		Log(L"开始保存...");
		auto path = DlgManager.ShowSaveFileDialog();
		if (path.empty()) return;
		if (path.find(L".layout") == wstring::npos) path = path + L".layout";
		Layout::ControlLayout.SaveLayoutFile(Conver::WCharToAChar(path), drawable->mSaveSet);
		Log(L"保存完毕...");
	});

	//Load
	baseWindow.AddClickFuncByID(158, [&]() {
		Log(L"载入中...");
		auto path = DlgManager.ShowOpenFileDialog();
	    baseWindow.Close();
		CloseHandle(ThreadHandle);   

		BaseWindow bsNew(Conver::WCharToAChar(path));
		auto newHandle = bsNew.Show();
		ThreadHandle = newHandle;

		WaitForSingleObject(newHandle,INFINITE);
		Log(L"载入完毕...");
	});

	Log(L"装载函数...");

	//LabelBox
	baseWindow.AddClickFuncByID(126, [&]() {
		drawable->mStateType = L"LabelBox";
		Log(L"选择LabelBox");
	});

	//Force
	baseWindow.AddClickFuncByID(127, [&]() {
		drawable->mStateType = L"ForceLabel";
		Log(L"选择ForceLabel");
	});

	//EditBox
	baseWindow.AddClickFuncByID(128, [&]() {
		drawable->mStateType = L"EditBox";
		Log(L"选择EditBox");
	});

	//ImgButton
	baseWindow.AddClickFuncByID(129, [&]() {
		drawable->mStateType = L"ImageButton";
		Log(L"选择ImageButton");
	});

	//StaticLabel
	baseWindow.AddClickFuncByID(130, [&]() {
		drawable->mStateType = L"StaticLabel";
		Log(L"选择StaticLabel");
	});

	//关闭
	baseWindow.AddClickFuncByID(152, [&]() {
		drawable->mStateType = L"CloseButton";
		Log(L"选择CloseButton");
	});

	//最小
	baseWindow.AddClickFuncByID(153, [&]() {
		drawable->mStateType = L"MiniButton";
		Log(L"选择MiniButton");
	});

	//最大
	baseWindow.AddClickFuncByID(154, [&]() {
		drawable->mStateType = L"MaxButton";
		Log(L"选择MaxButton");
	});

	//TitleBar
	baseWindow.AddClickFuncByID(159, [&]() {
		drawable->mStateType = L"TitleBar";
		Log(L"选择TitleBar");
	});

	//DrawAbleLabel
	baseWindow.AddClickFuncByID(125, [&]() {
		using namespace Conver;
		auto pControl = drawable->pForcedControl;
		if (pControl == nullptr)
		{
			baseWindow.SetRangeIDValue(141, 151, L"null");
		}
		else
		{
			ColorStruct tempColor = pControl->BackColor();
			baseWindow.SetValueByID(142, Format(L"%d", pControl->width()));
			baseWindow.SetValueByID(143, Format(L"%d", pControl->height()));
			baseWindow.SetValueByID(145, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->ForceColor();
			baseWindow.SetValueByID(146, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->HoverBackColor();
			baseWindow.SetValueByID(147, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->HoverForceColor();
			baseWindow.SetValueByID(148, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			baseWindow.SetValueByID(149, Format(L"%f", pControl->getBoardSize()));
			tempColor = pControl->getBoardColor();
			baseWindow.SetValueByID(150, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			baseWindow.SetValueByID(151, Format(L"%d", pControl->CanDrag()));
			baseWindow.SetValueByID(141, pControl->Text());
			baseWindow.SetValueByID(157, Format(L"%d", pControl->HaveBoard()));
		}
	});

	//更新属性
	baseWindow.AddClickFuncByID(155, [&]() {
		using namespace Layout;
		using namespace Conver;
		auto UpdateData = [&](Control * pControl) {
			//更新
			pControl->SetWidth(utoi(baseWindow.getValueByID(142)));
			pControl->SetHeight(utoi(baseWindow.getValueByID(143)));
			pControl->SetBackColor(ControlLayout.getColor(baseWindow.getValueByID(145)));
			pControl->SetForceColor(ControlLayout.getColor(baseWindow.getValueByID(146)));
			pControl->SetHoverBackColor(ControlLayout.getColor(baseWindow.getValueByID(147)));
			pControl->SetHoverForceColor(ControlLayout.getColor(baseWindow.getValueByID(148)));
			pControl->setBoardSize(utof(baseWindow.getValueByID(149)));
			pControl->setBoardColor(ControlLayout.getColor(baseWindow.getValueByID(150)));
			pControl->SetDrag(utob(baseWindow.getValueByID(151)));
			pControl->SetText(baseWindow.getValueByID(141));
			pControl->SetBoard(utob(baseWindow.getValueByID(157)));
		};

		auto pControl = DYCAST(DrawAbleLabel *, baseWindow.mListener.findElementByID(125))->pForcedControl;
		IS_RETURN(!pControl,);
		Log(L"开始更新...");
		auto id = pControl->getID();
		auto findControl = find_if(drawable->mSaveSet.begin(), drawable->mSaveSet.end(), [&id](Control * pControl) {
			if (pControl->getID() == id) return true;
			return false;
		});
		if (findControl == drawable->mSaveSet.end()) return;

		UpdateData(*findControl);
		UpdateData(pControl);
			
		drawable->SaveControl(1024.0f/drawable->width(),768.0f/drawable->height(),pControl);
		Log(L"更新完毕...");
	});

	Log(L"就绪");

	WaitForSingleObject(ThreadHandle, INFINITE);
	CloseHandle(ThreadHandle);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

    return 0;
}

