#include "Control.h"
#include <algorithm>

namespace LIB_CONTROL
{
	Control::~Control()
	{
	}

	bool Listener::attach(Control * pControl)
	{
		if(find_if(mpControl.begin(), mpControl.end(), [&pControl](Control * pCl)->bool {
			if (pCl == pControl) return true;
			return false;
		}) != mpControl.end()) return false;

		mpControl.emplace_back(pControl);
		return true;
	}

	bool Listener::detach(Control * pControl)  
	{
		list<Control *>::iterator it;
		it = find_if(mpControl.begin(), mpControl.end(), [&pControl](Control * pCl)->bool {
			if (pCl == pControl) return true;
			return false;
		});
		if(it == mpControl.end()) return false;
		mpControl.erase(it);
		return false;
	}

	void Listener::Draw()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->Draw(this);
		});
	}

	void Listener::Hover(POINT pt)
	{
		Control * pTempControl = NULL;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->SetInternal(false);
			if (Conver::PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				pTempControl = pControl;
				pControl->SetInternal(true);
				pTempControl->Hover(this, pt);
			}
		});
	}
		

	void Listener::LButtonDown()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->LButtonDown(this);
		});
	}

	void Listener::LButtonUp()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->LButtonUp(this);
		});
	}

	UINT Listener::HitTest( POINT pt)
	{
		Control * pTempControl = NULL;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			if (Conver::PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				pTempControl = pControl;
			}
		});
		if(!pTempControl)  return HTCLIENT;
		return pTempControl->HitTest(this, pt);
	}

	string Control::Text() const
	{
		return mText;
	}

	void Control::SetText(string text)
	{
		if (!text.empty()) mText = text;
	}

	COLORREF Control::ForceColor() const
	{
		return mForceColor;
	}

	void Control::SetForceColor(COLORREF color)
	{
		mForceColor = color;
	}

	COLORREF Control::BackColor() const
	{
		return mBackColor;
	}

	void Control::SetBackColor(COLORREF color)
	{
		mBackColor = color;
	}

	COLORREF Control::HoverBackColor() const
	{
		return mHonverBackColor;
	}

	void Control::SetHoverBackColor(COLORREF color)
	{
		mHonverBackColor = color;
	}

	COLORREF Control::HoverForceColor() const
	{
		return mHoverForceColor;
	}

	void Control::SetHoverForceColor(COLORREF color)
	{
		mHoverForceColor = color;
	}

	bool Control::IsVisible() const
	{
		return mVisible;
	}

	void Control::SetVisible(bool state)
	{
		mVisible = state;
	}

	RECT Control::getRect() const
	{
		return mRect;
	}

	void Control::AdjustRect(RECT decREct)
	{
		mRect = decREct;
	}

	UINT Control::getAlignType() const
	{
		return mAlignType;
	}

	void Control::setAlignType(UINT uType)
	{
		mAlignType = uType;
	}

	bool Control::IsMouseInteral() const
	{	
		return mMouseInternal?true:false;
	}

	void Control::SetInternal(bool MouseInternal)
	{
		mMouseInternal = MouseInternal;
	}

	Control::Control():Control("")
	{

	}
	
	Control::Control(string text, RECT rc):mRect(rc),mText(text),mVisible(true),mBackColor(RGB(45,45,48)),
		mForceColor(RGB(110,110,112)), mHonverBackColor(RGB(63, 63, 65)),mHoverForceColor(RGB(110, 110, 112)),
		mAlignType(ALIGN_CENTER_V|ALIGN_CENTER_H)
	{

	}

	Control::Control(RECT rc, string text, COLORREF forceColor, COLORREF backColor,COLORREF hoverBackColor):Control(text,rc)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
		SetHoverBackColor(hoverBackColor);
	}

	void Button::Draw(Listener * pListener)
	{

	}

	void Button::Hover(Listener * pListener,POINT pt)
	{

	}

	void Button::LButtonDown(Listener * pListener)
	{

	}

	void Button::LButtonUp(Listener * pListener)
	{

	}

	UINT Button::HitTest(Listener * pListener,POINT pt)
	{
		mMouseInternal = true;
		return HTCLIENT;
	}

	void TitleBar::Draw(Listener * pListener)
	{
		if (!mMouseInternal)
		{
			auto ret = DrawManager.DrawRectWithText(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor), ALIGN_DEFAULT, true);
			IS_ERROR_EXIT(!ret, "Draw current Rect failed!");
		}
		else
		{
			auto ret = DrawManager.DrawRectWithText(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mForceColor), ALIGN_DEFAULT, true);
			IS_ERROR_EXIT(!ret, "Draw hover failed!");
		}

	}

	void TitleBar::Hover(Listener * pListener,POINT pt)
	{
	}

	void TitleBar::LButtonDown(Listener * pListener)
	{
	}

	void TitleBar::LButtonUp(Listener * pListener)
	{
	}

	UINT TitleBar::HitTest(Listener * pListener, POINT pt)
	{
		return HTCAPTION;
	}

	TitleBar::TitleBar(string text, IPIC * pPic)
	{
		mText = text, mPic = pPic;
	}

	TitleBar::~TitleBar()
	{

	}
}
