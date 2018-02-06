#pragma once
#include <list>
#include <functional>
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

		COLORREF mHonverBackColor, mHoverForceColor;

		bool mVisible;

		UINT mAlignType;

		bool mMouseInternal;

	public:

		virtual void Draw(Listener * pListener) = 0;

		virtual void Hover(Listener * pListener, POINT pt) = 0;

		virtual void LButtonDown(Listener * pListener) = 0;

		virtual void LButtonUp(Listener * pListener) = 0;

		virtual UINT HitTest(Listener * pListener,POINT pt) = 0;

		string Text() const;

		void SetText(string text);

		COLORREF ForceColor() const;

		void SetForceColor(COLORREF color);

		COLORREF BackColor() const;

		void SetBackColor(COLORREF color);

		COLORREF HoverBackColor() const;

		void SetHoverBackColor(COLORREF color);

		COLORREF HoverForceColor() const;

		void SetHoverForceColor(COLORREF color);

		bool IsVisible() const;

		void SetVisible(bool state);

		RECT getRect() const;

		void AdjustRect(RECT decREct);

		UINT getAlignType() const;

		void setAlignType(UINT uType);

		bool IsMouseInteral() const;

		void SetInternal(bool MouseInternal);

		Control();

		Control(string text, RECT rc = {0,0});

		Control(RECT rc, string text, COLORREF forceColor, COLORREF backColor,COLORREF hoverBackColor);

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

		void Hover(POINT pt);

		void LButtonDown();

		void LButtonUp();

		UINT HitTest(POINT pt);
	};

	///<Button>∞¥≈•</Button>
	class Button : public Control
	{
	protected:
		COLORREF mBoardColor;

		bool mDrawBoard;
	public:
		void SetBoardColor(COLORREF cBoardColor);

		void NeedDrawBoard(bool bDrawBoard);

		virtual void Draw(Listener * pListener) override;

		virtual void Hover(Listener * pListener, POINT pt) override;

		virtual void LButtonDown(Listener * pListener) override;

		virtual void LButtonUp(Listener * pListener) override;

		virtual UINT HitTest(Listener * pListener,POINT pt) override;

		Button();
	};

	class ImageAdapter
	{
	protected:
		IPIC * pImage;
	public:
		bool SetImage(IPIC * img);

		bool LoadFromFile(wstring strFileName);
	};

	class CloseButton : public Button ,public ImageAdapter
	{
		void Draw(Listener * pListener) override final;

		void Hover(Listener * pListener, POINT pt) override final;

		void LButtonDown(Listener * pListener) override final;

		void LButtonUp(Listener * pListener) override final;
	};

	///<TitleBar>±ÍÃ‚¿∏</TitleBar>
	class TitleBar : public Control
	{
		IPIC * mPic;
	public:
		void Draw(Listener * pListener) override;

		void Hover(Listener * pListener, POINT pt) override;

		void LButtonDown(Listener * pListener) override;

		void LButtonUp(Listener * pListener) override;

		UINT HitTest(Listener * pListener,POINT pt) override;

		TitleBar(string text,IPIC * pPic);

		~TitleBar();
	};

}



