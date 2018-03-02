#pragma once
#include <list>
#include <algorithm>
#include "2Draw.h"
#include "Sprite.h"

namespace LIB_CONTROL
{
	using namespace std;

	using namespace MyMessage;

	class Listener;

	class ImageAdapter
	{
	protected:
		IPIC * pImage;
		
		bool bDrawImage;

		RECT mImgRec;

	public:

		bool SetImage(IPIC * img);

		bool LoadFromFile(wstring strFileName);

		void SetImgRect(RECT desRect);

		RECT ImgRect() const; 

		ImageAdapter();

		~ImageAdapter();
	};

	class Control: public ImageAdapter
	{

	protected:

		RECT mRect;

		string mText;

		COLORREF mForceColor, mBackColor;

		COLORREF mHonverBackColor, mHoverForceColor;

		bool mVisible;

		UINT mAlignType;

		bool mMouseInternal;

		bool mActive;

		bool mCanStretch; //拉伸控件

		UINT mControlTypeId;

		void SetID(UINT typeId);

	public:

		virtual void Draw(Listener * pListener) = 0;

		virtual void Hover(Listener * pListener, POINT pt) = 0;

		virtual UINT LButtonDown(Listener * pListener) = 0;

		virtual UINT LButtonUp(Listener * pListener) = 0;

		virtual UINT HitTest(Listener * pListener,POINT pt) = 0;
		
		virtual RECT & Sizing(POINT pointLeftTop, POINT pointRightBottom);

		bool Stretch(); //是否是一个随着窗口大小变化而改变的控件

		UINT TypeId() const;

		bool Active() const;

		void SetActive(bool bActive);

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

		LONG width() const;

		LONG height() const;

		Control();

		Control(string text, RECT rc = {0,0});

		Control(RECT rc, string text, COLORREF forceColor, COLORREF backColor,COLORREF hoverBackColor);

		virtual ~Control();
	};

	class ComposeControl:public Control
	{
	protected:

		list<Control *>  mControl;

	public:

		bool Add(Control * pControl);

		virtual void Draw(Listener * pListener);

		virtual UINT HitTest(Listener * pListener, POINT pt);

		virtual void Hover(Listener * pListener, POINT pt);

		virtual UINT LButtonDown(Listener * pListener);

		virtual UINT LButtonUp(Listener * pListener);
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

		UINT LButtonDown(POINT pt);

		UINT LButtonUp(POINT pt);

		UINT HitTest(POINT pt);

		list<Control *> & Obj();
	};

	///<Button>按钮</Button>
	class Button : public Control
	{
	protected:
		COLORREF mBoardColor;

		bool mDrawBoard;

		bool BDownInternal;

	public:

		void SetButtonDownInternal(bool isDownInternal);

		void SetBoardColor(COLORREF cBoardColor);

		void NeedDrawBoard(bool bDrawBoard);

		virtual void Draw(Listener * pListener) override;

		virtual void Hover(Listener * pListener, POINT pt) override;

		virtual UINT LButtonDown(Listener * pListener) override;

		virtual UINT LButtonUp(Listener * pListener) override;

		virtual UINT HitTest(Listener * pListener,POINT pt) override;

		Button();
	};

	class CloseButton : public Button 
	{
	public:

		CloseButton();

		void Draw(Listener * pListener) override final;

		void Hover(Listener * pListener, POINT pt) override final;

		UINT LButtonDown(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener) override final;

		UINT HitTest(Listener * pListener, POINT pt) override final;
	};

	class MiniButton : public Button
	{
	public:

		MiniButton();

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener) override final;

	};

	class MaxButton : public Button
	{
	public:

		MaxButton();

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener) override final;

	};

	class RestoreButton : public Button
	{
	public:

		RestoreButton();

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener) override final;

	};

	///<TitleBar>标题栏</TitleBar>
	class TitleBar : public Control
	{
		Sprite::Sprite mIconSprite;

	public:
		void Draw(Listener * pListener) override;

		void Hover(Listener * pListener, POINT pt) override;

		UINT LButtonDown(Listener * pListener) override;

		UINT LButtonUp(Listener * pListener) override;

		UINT HitTest(Listener * pListener,POINT pt) override;

		TitleBar(string text,wstring filename);

		~TitleBar();
	};

}



