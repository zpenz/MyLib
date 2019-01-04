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

class TestWindow :public BaseWindow {
public:
	void AfterCreate() override {
		//初始化D2D1.0
		IS_RETURN_ERROR(!DrawManager.InitManager(), , "初始化D2D错误!");
		IS_RETURN_ERROR(!DrawManager.SetRenderTarget(mBaseHwnd), , "设置RenderTarget失败!");
		DrawManager.UseTempRenderTarget();

		CaretManager.attrach(mBaseHwnd);
		CaretManager.Color(RGB(0, 0, 0));

		//窗口完全矩形..
		//auto Rgn = CreateRectRgn(0,0,mWidth,mHeight);
		//SetWindowRgn(mBaseHwnd, Rgn,true);
		//DeleteObject(Rgn);

		mListener.attachWindow(mBaseHwnd);
		//加载所有控件
		IS_ERROR_EXIT(!Layout::ControlLayout.LoadLayoutFile(mLayoutFileName.empty() ? DEFAULT_LAYOUT : mLayoutFileName.c_str(), &mListener), "加载布局文件失败...");
		auto background = mListener.findElementByID(0);
		IS_ERROR_EXIT(!background, "没有找到背景控件!");
		SetWidth(background->width());
		SetHeight(background->height());
		DrawManager.ReSize(background->width(), background->height());
	};
	void OnDraw() override {
		DrawManager.Clear(MyColor::Black);
		mListener.Draw();
		RECT windowRect;
		GetWindowRect(mBaseHwnd, &windowRect);
		Conver::ScreenToClientRc(mBaseHwnd, windowRect);
		DrawManager.DrawRectangle(windowRect, COLOREX(RGB(57, 130, 255)), false);
		DrawManager.Present(&windowRect);
	};
};

int CALLBACK WinMain(HINSTANCE hInstance,
	 HINSTANCE hPrevInstance,
	 LPSTR lpCmdLine,
	int nShowCmd)
{
	using namespace CommonItem;
	using namespace MutexLock;
	 
	TestWindow testWindow;
	testWindow.UseWrapper();
	auto ThreadHandle = testWindow.Show();

	Sleep(2000);

	auto drawable = DYCAST(DrawAbleLabel *, testWindow.findElementByID(125));
	
	if(drawable) drawable->SaveRangleControlByID(160, 163, &testWindow.mListener);
	
	auto Log = [&](wstring info) {
		//状态..
		IS_RETURN(!testWindow.findElementByID(123),false);
		testWindow.findElementByID(123)->SetText(info);
		Sleep(200);
		return true;
	};

	//SAVE
	testWindow.AddClickFuncByID(120, [&]() {
		Log(L"开始保存...");
		auto path = DlgManager.ShowSaveFileDialog();
		if (path.empty()) return;
		if (path.find(L".layout") == wstring::npos) path = path + L".layout";
		Layout::ControlLayout.SaveLayoutFile(Conver::WCharToAChar(path), drawable->mSaveSet);
		Log(L"保存完毕...");
	});

	//Load
	testWindow.AddClickFuncByID(158, [&]() {
		Log(L"载入中...");
		auto path = DlgManager.ShowOpenFileDialog();
		
		drawable->LoadControl("");
		Log(L"载入完毕...");
	});

	Log(L"装载函数...");

	//LabelBox
	testWindow.AddClickFuncByID(126, [&]() {
		drawable->mStateType = L"LabelBox";
		Log(L"选择LabelBox");
	});

	//Force
	testWindow.AddClickFuncByID(127, [&]() {
		drawable->mStateType = L"ForceLabel";
		Log(L"选择ForceLabel");
	});

	//EditBox
	testWindow.AddClickFuncByID(128, [&]() {
		drawable->mStateType = L"EditBox";
		Log(L"选择EditBox");
	});

	//ImgButton
	testWindow.AddClickFuncByID(129, [&]() {
		drawable->mStateType = L"ImageButton";
		Log(L"选择ImageButton");
	});

	//StaticLabel
	testWindow.AddClickFuncByID(130, [&]() {
		drawable->mStateType = L"StaticLabel";
		Log(L"选择StaticLabel");
	});

	//关闭
	testWindow.AddClickFuncByID(152, [&]() {
		drawable->mStateType = L"CloseButton";
		Log(L"选择CloseButton");
	});

	//最小
	testWindow.AddClickFuncByID(153, [&]() {
		drawable->mStateType = L"MiniButton";
		Log(L"选择MiniButton");
	});

	//最大
	testWindow.AddClickFuncByID(154, [&]() {
		drawable->mStateType = L"MaxButton";
		Log(L"选择MaxButton");
	});

	//TitleBar
	testWindow.AddClickFuncByID(159, [&]() {
		drawable->mStateType = L"TitleBar";
		Log(L"选择TitleBar");
	});

	//DrawAbleLabel
	testWindow.AddClickFuncByID(125, [&]() {
		using namespace Conver;
		auto pControl = drawable->pForcedControl;
		if (pControl == nullptr)
		{
			testWindow.SetRangeIDValue(141, 151, L"null");
		}
		else
		{
			ColorStruct tempColor = pControl->BackColor();
			testWindow.SetValueByID(142, Format(L"%d", pControl->width()));
			testWindow.SetValueByID(143, Format(L"%d", pControl->height()));
			testWindow.SetValueByID(145, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->ForceColor();
			testWindow.SetValueByID(146, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->HoverBackColor();
			testWindow.SetValueByID(147, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			tempColor = pControl->HoverForceColor();
			testWindow.SetValueByID(148, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			testWindow.SetValueByID(149, Format(L"%f", pControl->getBoardSize()));
			tempColor = pControl->getBoardColor();
			testWindow.SetValueByID(150, Format(L"%d %d %d", tempColor.r, tempColor.g, tempColor.b));
			testWindow.SetValueByID(151, Format(L"%d", pControl->CanDrag()));
			testWindow.SetValueByID(141, pControl->Text());
			testWindow.SetValueByID(157, Format(L"%d", pControl->HaveBoard()));
		}
	});

	//更新属性
	testWindow.AddClickFuncByID(155, [&]() {
		using namespace Layout;
		using namespace Conver;
		auto UpdateData = [&](Control * pControl) {
			//更新
			pControl->SetWidth(utoi(testWindow.getValueByID(142)));
			pControl->SetHeight(utoi(testWindow.getValueByID(143)));
			pControl->SetBackColor(ControlLayout.getColor(testWindow.getValueByID(145)));
			pControl->SetForceColor(ControlLayout.getColor(testWindow.getValueByID(146)));
			pControl->SetHoverBackColor(ControlLayout.getColor(testWindow.getValueByID(147)));
			pControl->SetHoverForceColor(ControlLayout.getColor(testWindow.getValueByID(148)));
			pControl->setBoardSize(utof(testWindow.getValueByID(149)));
			pControl->setBoardColor(ControlLayout.getColor(testWindow.getValueByID(150)));
			pControl->SetDrag(utob(testWindow.getValueByID(151)));
			pControl->SetText(testWindow.getValueByID(141));
			pControl->SetBoard(utob(testWindow.getValueByID(157)));
		};

		auto pControl = DYCAST(DrawAbleLabel *, testWindow.mListener.findElementByID(125))->pForcedControl;
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
			
		drawable->SaveControl(1,1,pControl);
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

