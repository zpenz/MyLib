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

	void Listener::Hover()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->Hover(this);
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

	void Listener::HitTest()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->HitTest(this);
		});
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
		return mMouseInternal;
	}
	
	Control::Control(RECT rc,string text):mRect(rc),mText(text),mVisible(true),mBackColor(RGB(45,45,48)),
		mForceColor(RGB(110,110,112)),mAlignType(ALIGN_CENTER_V|ALIGN_CENTER_H)
	{

	}

	Control::Control(RECT rc, string text, COLORREF forceColor, COLORREF backColor):Control(rc,text)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
	}

	void Button::Draw(Listener * pListener)
	{

	}

	void Button::Hover(Listener * pListener)
	{

	}

	void Button::LButtonDown(Listener * pListener)
	{

	}

	void Button::LButtonUp(Listener * pListener)
	{

	}
	bool Button::HitTest(Listener * pListener)
	{
		return false;
	}
}
