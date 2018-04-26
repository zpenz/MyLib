#include "Caret.h"
#define DEFAULT_CARET_START 3
using namespace Conver;

bool MyCaret::attrach(HWND hWnd)
{
	IS_RETURN_ERROR(!hWnd,false,"attrached window null")
	mAttachWindow = hWnd;
	InitCaret();
	return true;
}

//////////////////////////////////////////////////////////////////////////Caret
POINT MyCaret::PointOfDraw()
{
	return mCaretPos;
}

int MyCaret::getIndex() const
{
	return mInsertPos;
}

void MyCaret::setIndex(int newIndex)
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
	if (mInsertPos < -1) mInsertPos++;
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
			Sleep(STCAST(DWORD, 1.0 / pCaret->Time() * 1000));
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
		DrawManager.DrawLine(currentCaretPoint, Conver::Point(0,-1 * currentCaretHeight)+ currentCaretPoint,
			mCaretColor, STCAST(float,mControlWidth));   
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
	mCaretColor = RGB(0,0,0);
	mInsertPos = 0;
	isTrial = isHead = false;
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


void MyCaret::AdjustPos(RECT layoutBox, TextLayout * pTestMatric,POINT * TestPoint,bool SkipThis)
{
	if (pTestMatric == nullptr)//默认光标位置在左边
	{
		CaretManager.ChangeCaretPos(Point(layoutBox.left+ DEFAULT_CARET_START,layoutBox.bottom));
		return;
	}

	auto tempPoint = TestPoint;
	if (TestPoint == nullptr) tempPoint = &mCaretPos;

	float x, y;
	BOOL isTrail, inside;
	x = STCAST(float,tempPoint->x - LeftTopPoint(layoutBox).x);
	y = STCAST(float,tempPoint->y - LeftTopPoint(layoutBox).y);
	HitTestMatric Matrics;
	auto ret = pTestMatric->HitTestPoint(x, y, &isTrail, &inside, &Matrics);
	IS_RETURN_ERROR(FAILED(ret), , "Caret HitTestPoint error!");
	int hitTextPos = Matrics.textPosition;
	if (hitTextPos == -1) isHead = true;
	
	if (isTrail || SkipThis)
	{
		x = Matrics.left + Matrics.width;
	}
	else
	{
		x = Matrics.left;
		hitTextPos--;
	}

	y = Matrics.top + height();
	CaretManager.setIndex(hitTextPos);
	CaretManager.ChangeCaretPos(Point(STCAST(int,x),STCAST(int,y)) + LeftTopPoint(layoutBox));
}

POINT MyCaret::SetCaretPosEx(RECT layoutBox, TextLayout * pTestMatric, int index, bool isTrailingHit)
{
	if (pTestMatric == nullptr || index == 0)
	{
		CaretManager.ChangeCaretPos(Point(layoutBox.left + DEFAULT_CARET_START, layoutBox.bottom));
		return Point(layoutBox.left + DEFAULT_CARET_START, layoutBox.bottom);
	}

	HitTestMatric Matrics;
	float x, y;
	pTestMatric->HitTestTextPosition(index, isTrailingHit,&x ,&y, &Matrics);
	CaretManager.ChangeCaretPos(Point(STCAST(int, x), STCAST(int, y)+ RECTHEIGHT(layoutBox)) + LeftTopPoint(layoutBox));
	return Point(STCAST(int, x), STCAST(int, y)) + LeftTopPoint(layoutBox);
}

void MyCaret::Color(COLORREF caretColor)
{
	mCaretColor = caretColor;
}

COLORREF MyCaret::getColor()
{
	return mCaretColor;
}