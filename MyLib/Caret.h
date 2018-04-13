#pragma once
#include "2Draw.h"

#define ALIGN_UPDPWNDISTANCE 4

#define ALIGN_LEFTDISTANCE 4

class MyCaret
{

protected:

	POINT mCaretPos;

	bool mThisFrameShouldDraw;

	UINT mControlWidth, mControlHeight;

	float mBlinkTime;

	bool mSubThreadAlreayRun;

	bool mDead;

	bool mHide;

	int mInsertPos; 

	HWND mAttachWindow;

	COLORREF mCaretColor;

	SINGLE_INSTANCE(MyCaret)

	void DrawCaret();

public:

	POINT currentCaretPoint;

	UINT currentCaretHeight;

	void Render();

	UINT width();

	UINT height();
	
	bool attrach(HWND hWnd); //must call first

	bool isTrial; //β��

	bool isHead; //ͷ��

	POINT PointOfDraw();

	int getIndex() const;//�����������..

	void setIndex(int newIndex);

	void IncIndex(); //++

	void DecIndex(); //--

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

	POINT pos();

	void ChangeCaretPos(POINT newCaretPos);

	void ChangeCaretSize(UINT width, UINT height); /// re-create caret

	///<summary>�ѹ�����õ����ֵĺ��� pTestPoint Ĭ��Ϊ���λ��</summary>
	void AdjustPos(RECT layoutBox, TextLayout * pTestMatric,POINT * pTestPoint = nullptr, bool SkipThis = false);

	///<summary>���ù����ȷλ��!</summary><param name = "index">"����λ��"</param>
	POINT SetCaretPosEx(RECT layoutBox, TextLayout * pTestMatric,int index,bool isTrailingHit = true);

	void Color(COLORREF caretColor);

	COLORREF getColor();
};

#define CaretManager MyCaret::getInstance() 
