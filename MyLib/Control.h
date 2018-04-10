#pragma once
#include <list>
#include <algorithm>
#include <functional>
#include "2Draw.h"
#include "Sprite.h"
#include "Caret.h"
#include "reflect.h"

namespace LIB_CONTROL
{
	using namespace std;

	using namespace MyMessage;

	using namespace REFLECTION;

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

	class DragAdapter
	{
	protected:

		bool mCanDrag; //Drag

		bool mbDraging; //Draging

		int mLeft, mTop;

		virtual void Drag(Listener * pListener, int dx, int dy) = 0; ///dx dy 偏移

	public:
		bool IsDraging();

		void Stop();

		void SetDrag(bool DragState);

		DragAdapter();
	};

	template<class IDType>
	class IDAdapter
	{
		IDType mID;
	public:
		IDType getID() { return mID; }

		void SetID(IDType id) { mID = id; }

	};



	class Control: public ImageAdapter,public DragAdapter,public IDAdapter<UINT>
	{

	protected:

		RECT mRect;

		wstring mText;

		COLORREF mForceColor, mBackColor;

		COLORREF mHonverBackColor, mHoverForceColor;

		COLORREF mBoardColor;

		bool mVisible;

		bool mMouseInternal;

		bool mActive;

		bool mCanStretch; //Stretch

		bool mBDownInternal;

		POINT mouseDragStartPoint;

		bool mOwnCaret;

		bool mFocusCaret;

		POINT mCaretPos; 

		TextLayout * mpTextpLayout;

	public:

		function<void(void)> mClickFunc;

		virtual UINT HitTest(Listener * pListener, POINT pt);

		virtual void Draw(Listener * pListener);

		virtual void Hover(Listener * pListener, POINT pt);

		virtual UINT LButtonDown(Listener * pListener,POINT pt);

		virtual UINT LButtonUp(Listener * pListener,POINT pt);

		virtual void MouseMove(Listener * pListener,POINT pt); //mouse move ///pt是关于Client的

		virtual void InputChar(Listener * pListener, wchar_t cUnicode);

		virtual void Focus(Listener * pListener,HWND hWnd);

		virtual void KillFocus(Listener * pListener);

		virtual void Sizing(RECT newRect);

		void Drag(Listener * pListener, int dx, int dy) override;

		bool HaveCaret();

		void SetCaretState(bool state);

		bool Stretch(); //是否是一个随着窗口大小变化而改变的控件

		bool Active() const;

		void SetActive(bool bActive);

		wstring Text() const;

		void SetText(wstring text);

		void SetBoardColor(COLORREF color);

		COLORREF getBoardColor();

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

		bool IsMouseInteral() const;

		void SetInternal(bool MouseInternal);

		LONG width() const;

		LONG height() const;

		Control();

		Control(wstring text, RECT rc = {0,0});

		Control(wstring text, RECT rc,COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);

		virtual ~Control();
	};

	
	class Listener
	{
	protected:

		list<Control *> mpControl;

		HWND mListenedWindow;

	public:

		bool attachWindow(HWND hWnd);

		bool attach(Control * pControl);

		bool detach(Control * pControl);

		void Draw();

		void InputChar(wchar_t c);

		void Hover(POINT pt);

		UINT LButtonDown(POINT pt);

		UINT LButtonUp(POINT pt);

		UINT HitTest(POINT pt);

		void MouseMove(POINT pt);

		void ChangeSize(RECT newRect);

		list<Control *> & Obj();
	};

	///<ButtonInterface>按钮接口</ButtonInterface>
	class ButtonInterface : public Control
	{
	protected:
		COLORREF mBoardColor;

		bool mDrawBoard;

	public:

		void SetButtonDownInternal(bool isDownInternal);

		void SetBoardColor(COLORREF cBoardColor);

		void NeedDrawBoard(bool bDrawBoard);

		virtual void Draw(Listener * pListener) override;

		virtual UINT HitTest(Listener * pListener,POINT pt) override;

		ButtonInterface();

		ButtonInterface(wstring text,RECT rc);

		ButtonInterface(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);
	};

	///默认按钮
	class Button :public ButtonInterface, public REFLECT<Button>
	{
	public:
		Button();
	};

	class CloseButton : public ButtonInterface 
		,public REFLECT<CloseButton>
	{
	public:

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener,POINT pt) override final;

		UINT HitTest(Listener * pListener, POINT pt) override final;

		void Sizing(RECT newRect) override;

		CloseButton();

		CloseButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);

	};

	class MiniButton : public ButtonInterface
		, public REFLECT<MiniButton>
	{
	public:

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener,POINT pt) override final;

		void Sizing(RECT newRect) override;

		MiniButton();

		MiniButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);
	};

	class MaxButton : public ButtonInterface // max and restore button...
		, public REFLECT<MaxButton>
	{
	protected:

		void DrawMaxImg();

		void DrawRestoreImg();

		bool isMax;

	public:

		void setState(bool isMax);

		void Draw(Listener * pListener) override final;

		UINT LButtonUp(Listener * pListener,POINT pt) override final;

		void Sizing(RECT newRect) override;

		MaxButton();

		MaxButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);
	};

	///<ImageButton>图形按钮</ImageButton>
	class ImageButton : public ButtonInterface
	{
		IPIC * pHonverImage;

	public:

		void Draw(Listener * pListener) override;

		ImageButton(wstring picLoc,RECT rImgRect);

		ImageButton(IPIC * pBitmap, RECT rImgRect);
	};

	///<TitleBar>标题栏</TitleBar>
	class TitleBar : public Control
		,public REFLECT<TitleBar>
	{
		Sprite::Sprite mIconSprite[2];

	public:
		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;

		void Sizing(RECT newRect) override final;

		TitleBar();

		TitleBar(wstring text, RECT rc);

		TitleBar(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);

		~TitleBar();
	};

	///<EditBox>编辑框</EditBox>
	class EditBox : public Control
		, public REFLECT<EditBox>
	{
		float mFrontSize;

		wstring mFrontName;

	public:

		void ChangeFrontSize(float newSize);

		void ChangeFrontName(wstring newFrontName);

		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;

		EditBox();

		EditBox(RECT rc, string defaultText = "");

		EditBox(wstring defaultText, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);
	};

	///<LabelBox>标签</LabelBox>
	class LabelBox :public Control
		, public REFLECT<LabelBox>
	{
	public:
		LabelBox();

		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;
	};

	///<ComposeControl>组合控件</ComposeControl>
	class ComposeControl :public Control
	{
	protected:

		list<ComposeControl *>  mControl; //mControl 所有坐标相对于 最外面的包围矩形

		Listener * pmListener;

	public:

		bool Attach(Listener * pListener);

		bool Add(Control * pControl);

		virtual void Draw(Listener * pListener);

		virtual UINT HitTest(POINT pt);

		virtual void Hover(POINT pt);

	};
}



