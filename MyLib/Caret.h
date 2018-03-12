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

	COLORREF mCaretColor;

	SINGLE_INSTANCE(MyCaret)

public:

	POINT PointOfDraw();

	bool IsCaretExist();

	bool IsThisFrameShouldDraw();

	void ChangeFrame();

	void DrawCaret(HWND hWnd);

	void InitCaret(HWND hWnd);

	void DestroyCaret();

	void HideCaret();

	void ShowCaret();

	bool IsHide();

	void SetBlinkTime(float timeIntral);

	float Time();

	void ChangeCaretPos(POINT newCaretPos);

	void ChangeCaretSize(UINT width, UINT height);

	void Color(COLORREF caretColor);

	COLORREF getColor();
};

#define CaretManager MyCaret.getInstance()
