#pragma once
#include <list>
#include "2Draw.h"

namespace LIB_CONTROL
{
	using namespace std;

	class Listener;

	class Control
	{
	protected:
		RECT mRect;

		string mText;

		COLORREF mForceColor, mBackColor;

		bool mVisible;

		UINT mAlignType;

		bool mMouseInternal;

	public:

		virtual void Draw(Listener * pListener) = 0;

		virtual void Hover(Listener * pListener) = 0;

		virtual void LButtonDown(Listener * pListener) = 0;

		virtual void LButtonUp(Listener * pListener) = 0;

		virtual bool HitTest(Listener * pListener) = 0;

		string Text() const;

		void SetText(string text);

		COLORREF ForceColor() const;

		void SetForceColor(COLORREF color);

		COLORREF BackColor() const;

		void SetBackColor(COLORREF color);

		bool IsVisible() const;

		void SetVisible(bool state);

		RECT getRect() const;

		void AdjustRect(RECT decREct);

		UINT getAlignType() const;

		void setAlignType(UINT uType);

		bool IsMouseInteral() const;

		Control(RECT rc, string text);

		Control(RECT rc, string text, COLORREF forceColor, COLORREF backColor);

		virtual ~Control();
	};

	class Listener
	{
	protected:

		list<Control *> mpControl;

	public:
		bool attach(Control * pControl);

		bool detach(Control * pControl);

		void Draw();

		void Hover();

		void LButtonDown();

		void LButtonUp();

		void HitTest();
	};

	class Button : public Control
	{
	public:
		void Draw(Listener * pListener) override;

		void Hover(Listener * pListener) override;

		void LButtonDown(Listener * pListener) override;

		void LButtonUp(Listener * pListener) override;

		bool HitTest(Listener * pListener) override;
	};

}



