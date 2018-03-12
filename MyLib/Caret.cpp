#include "stdafx.h"
#include "Caret.h"


bool MyCaret::attrach(HWND hWnd)
{
	IS_RETURN_ERROR(!hWnd,false,"attrached window null")
	mAttachWindow = hWnd;
	return true;
}

//////////////////////////////////////////////////////////////////////////Caret
POINT MyCaret::PointOfDraw()
{
	return mCaretPos;
}

bool MyCaret::IsCaretExist()
{
	return !mDead;
}

bool MyCaret::IsThisFrameShouldDraw()
{
	return false;
}

void MyCaret::ChangeFrame()
{
	mThisFrameShouldDraw = !mThisFrameShouldDraw;
}

void MyCaret::DrawCaret()
{
	auto subThread = [](LPVOID lpParam)->DWORD
	{
		auto pCaret = (MyCaret*)lpParam;
		while (pCaret->IsCaretExist())
		{
			if (pCaret->IsThisFrameShouldDraw() && pCaret->IsHide() != true)
			{
				DrawManager.DrawLine(pCaret->PointOfDraw(), Conver::Point(), pCaret->getColor());
			}

			Sleep(static_cast<DWORD>(pCaret->Time()*0.001));
			pCaret->ChangeFrame();
		}
		return 0;
	};

	if (mSubThreadAlreayRun) return;

	DWORD threadId = 0;
	auto handle = CreateThread(NULL, 0, subThread, this, 0, &threadId);
	IS_RETURN_ERROR(!handle, , "Caret线程创建失败!");
	CloseHandle(handle);

	mSubThreadAlreayRun = true;
}

bool MyCaret::InitCaret()
{
	mWidth = 2;
	mHeight = 100;
	IS_RETURN_ERROR(CreateCaret(mAttachWindow, NULL, mWidth, mHeight),false,"创建Caret失败!");
	mDead = false;
	mHide = true;
	mSubThreadAlreayRun = false;
	mCaretColor = RGB(199,199,199);

	DrawCaret();
	return true;
}

void MyCaret::DestroyCaret()
{
	mDead = true;
	::DestroyCaret();
	
}

void MyCaret::HideCaret()
{
	mHide = true;
}

void MyCaret::ShowCaret()
{
	mHide = false;
}

bool MyCaret::IsHide()
{
	return mHide;
}

void MyCaret::SetBlinkTime(float timeIntral)
{
	if (timeIntral == 0) timeIntral = 1;
	mBlinkTime = timeIntral;
}

float MyCaret::Time()
{
	return mBlinkTime;
}

void MyCaret::ChangeCaretPos(POINT newCaretPos)
{
	mCaretPos = newCaretPos;
	SetCaretPos(mCaretPos.x, mCaretPos.y);
}

void MyCaret::ChangeCaretSize(UINT width, UINT height)
{
	if (width == 0 && height == 0) return;
	if (width == 0) { mHeight = height; return; }
	if (height == 0) { mWidth = width;  return; }
	mWidth = width; mHeight = height;

	DestroyCaret();
	CreateCaret(mAttachWindow,NULL,width,height);
}

void MyCaret::Color(COLORREF caretColor)
{
	mCaretColor = caretColor;
}

COLORREF MyCaret::getColor()
{
	return mCaretColor;
}