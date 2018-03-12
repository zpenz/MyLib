#pragma once
#include "2Draw.h"

class MyCaret
{
protected:

	POINT mCaretPos;

	bool mThisFrameShouldDraw;

	UINT mWidth, mHeight;

	float mBlinkTime;

	bool mSubThreadAlreayRun;

	bool mDead;

	bool mHide;

	HWND mAttachWindow;

	COLORREF mCaretColor;

	SINGLE_INSTANCE(MyCaret)

	void DrawCaret();

public:

	void Render();

	UINT width();

	UINT height();
	
	bool attrach(HWND hWnd); //must call first

	POINT PointOfDraw();

	bool IsCaretExist();

	bool IsThisFrameShouldDraw();

	void ChangeFrame();

	bool InitCaret(); // after drawmanager init.

	void DestroyCaret();

	void HideCaret();

	void ShowCaret();

	bool IsHide();

	void SetBlinkTime(float timeIntral);

	float Time();

	void ChangeCaretPos(POINT newCaretPos);

	void ChangeCaretSize(UINT width, UINT height); /// re-create caret

	void Color(COLORREF caretColor);

	COLORREF getColor();
};

#define CaretManager MyCaret::getInstance() 
