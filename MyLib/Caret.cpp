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

UINT32 MyCaret::getIndex() const
{
	return mInsertPos;
}

void MyCaret::setIndex(UINT32 newIndex)
{
	mInsertPos = newIndex;
}

void MyCaret::IncIndex()
{
	mInsertPos++;
}

void MyCaret::DecIndex()
{
	mInsertPos--;
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
	// multi thread... 
	auto subThread = [](LPVOID lpParam)->DWORD
	{
		auto pCaret = (MyCaret*)lpParam;
		while (pCaret->IsCaretExist())
		{
			Sleep(static_cast<DWORD>(static_cast<int>(1.0 / pCaret->Time() * 1000)));
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

void MyCaret::Render()
{
	currentCaretHeight = height() - ALIGN_UPDPWNDISTANCE * 2;
	currentCaretPoint = Conver::Point(0, -1 * ALIGN_UPDPWNDISTANCE) + mCaretPos;
	if(!mHide && mThisFrameShouldDraw)
	{
		DrawManager.DrawLine(currentCaretPoint, Conver::Point(0,-currentCaretHeight)+ currentCaretPoint,
			mCaretColor, static_cast<float>(mControlWidth));
	}
}

UINT MyCaret::width()
{
	return mControlWidth;
}

UINT MyCaret::height()
{
	return mControlHeight;
}

bool MyCaret::InitCaret()
{
	mControlWidth = 2;
	mControlHeight = 100;
	IS_RETURN_ERROR(!CreateCaret(mAttachWindow, NULL, mControlWidth, mControlHeight),false,"创建Caret失败!");
	mDead = false;
	mHide = true;
	mSubThreadAlreayRun = false;
	mBlinkTime = 4;
	mCaretColor = RGB(199,199,199);
	mInsertPos = 0;

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
	DrawCaret();
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

POINT MyCaret::pos()
{
	return mCaretPos;
}

void MyCaret::ChangeCaretPos(POINT newCaretPos)
{
	mCaretPos = newCaretPos;
	SetCaretPos(mCaretPos.x, mCaretPos.y);
}

void MyCaret::ChangeCaretSize(UINT width, UINT height)
{
	if (width == 0 && height == 0) return;
	if (width == 0) { mControlHeight = height; return; }
	if (height == 0) { mControlWidth = width;  return; }
	mControlWidth = width; mControlHeight = height;

	DestroyCaret();
	CreateCaret(mAttachWindow,NULL,width,height);
}


void MyCaret::AdjustPos(RECT layoutBox, TextLayout * pTestMatric,POINT * TestPoint)
{
	using namespace Conver;

	auto tempPoint = TestPoint;
	if (TestPoint == nullptr) tempPoint = &mCaretPos;

	float x, y;
	BOOL isTrail, inside;
	x = STCAST(float,tempPoint->x - LeftTopPoint(layoutBox).x);
	y = STCAST(float,tempPoint->y - LeftTopPoint(layoutBox).y);
	HitTestMatric * pMatrics = new HitTestMatric(); //must create 
	auto ret = pTestMatric->HitTestPoint(x, y, &isTrail, &inside, pMatrics);
	IS_RETURN_ERROR(FAILED(ret), , "Caret HitTestPoint error!");
	CaretManager.setIndex(pMatrics->textPosition);
	x = pMatrics->left + pMatrics->width; y = pMatrics->top + height();
	CaretManager.ChangeCaretPos(Point(x, y) + LeftTopPoint(layoutBox));
	SAFE_DELETE(pMatrics);
}

void MyCaret::Color(COLORREF caretColor)
{
	mCaretColor = caretColor;
}

COLORREF MyCaret::getColor()
{
	return mCaretColor;
}