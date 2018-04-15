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
		bool CanDrag();
		void Stop();
		void SetDrag(bool DragState);
		DragAdapter();
	};

	template<class IDType>
	class IDAdapter
	{
		IDType mID;
	public:
		IDType & getID() { return mID; }
		void SetID(IDType id) { mID = id; }
	};

	class ReadAble
	{
	protected:
		bool mReadOnly;
	public:
		bool isReadOnly();
		void setReadAccess(bool readAccess);
		ReadAble();
	};

	template<class BoardType>
	class BoardAdapter
	{
	protected:
		bool mHaveBoard;
		BoardType mBoardSize;
		COLORREF mBoardColor;
	public:
		bool HaveBoard() { return mHaveBoard; }
		void SetBoard(bool haveBoard) { mHaveBoard = haveBoard; }
		void setBoardSize(BoardType boardSize) { mBoardSize = boardSize; }
		BoardType & getBoardSize() { return mBoardSize; }
		const COLORREF & getBoardColor() { return mBoardColor; }
		void setBoardColor(COLORREF boardColor) {mBoardColor = boardColor;}
		BoardAdapter():mHaveBoard(false),mBoardSize(1.0),mBoardColor(RGB(255,255,255)) {}
	};

	class Control: public ImageAdapter,public DragAdapter,
		public ReadAble,public IDAdapter<UINT>,public BoardAdapter<float>
	{

	protected:

		RECT mRect;

		wstring mText;

		COLORREF mForceColor, mBackColor;

		COLORREF mHonverBackColor, mHoverForceColor;

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

		wstring mClassName;

	public:

		void SetClassName(wstring className);

		wstring getClassName();

		function<void(void)> mClickFunc;

		virtual UINT HitTest(Listener * pListener, POINT pt);

		virtual void Draw(Listener * pListener);

		virtual void Hover(Listener * pListener, POINT pt);

		virtual UINT LButtonDown(Listener * pListener,POINT pt);

		virtual UINT LButtonUp(Listener * pListener,POINT pt);

		virtual void MouseMove(Listener * pListener,POINT pt); //mouse move ///pt是关于Client的

		virtual void InputChar(Listener * pListener, wchar_t cUnicode);

		virtual void Focus(Listener * pListener);

		virtual void KillFocus(Listener * pListener);

		virtual void Sizing(RECT newRect);

		void DrawBoard(Listener * pListener);

		void Drag(Listener * pListener, int dx, int dy) override;

		bool HaveCaret();

		void SetCaretState(bool state);

		bool Stretch(); //是否是一个随着窗口大小变化而改变的控件

		bool Active() const;

		void SetActive(bool bActive);

		wstring Text() const;

		void SetText(wstring text);

		COLORREF & ForceColor() ;

		void SetForceColor(COLORREF color);

		COLORREF & BackColor() ;

		void SetBackColor(COLORREF color);

		COLORREF & HoverBackColor() ;

		void SetHoverBackColor(COLORREF color);

		COLORREF & HoverForceColor() ;

		void SetHoverForceColor(COLORREF color);

		bool IsVisible() const;

		void SetVisible(bool state);

		RECT & getRect() ;

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

	template<class StackType>
	class Stack
	{
		UINT mStackCount;
		list<StackType>	mStackData;
	public:
		void count() { return mStackCount; }
		void Push(StackType data) { mStackData.push_back(data); mStackCount++; }
		StackType Pop() 
		{
			if (mStackCount == 0) return nullptr;
			mStackCount--;
			return *(--mStackData.end());
		}
	};

	class Listener
	{
	protected:

		list<Control *> mpControl;

		HWND mListenedWindow;

	public:

		Control * findElementByID(UINT id);

		bool AddClickFuncByID(UINT id,function<void(void)> pFunc,bool rewrite = false);

		bool attachWindow(HWND hWnd);

		bool attach(Control * pControl);

		bool detach(Control * pControl);

		void Draw();

		void InputChar(wchar_t c);

		void Hover(POINT pt);

		UINT LButtonDown(POINT pt);

		UINT LButtonUp(POINT pt);

		void MouseMove(POINT pt);

		UINT HitTest(POINT pt);

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
	class ImageButton : public ButtonInterface,
		public REFLECT<ImageButton>
	{
		IPIC * pHonverImage;

	public:

		void Draw(Listener * pListener) override;

		ImageButton();

		ImageButton(wstring picLoc,RECT rImgRect);

		ImageButton(IPIC * pBitmap, RECT rImgRect);

		~ImageButton();
	};

	///<TitleBar>标题栏</TitleBar>
	class TitleBar : public Control
		,public REFLECT<TitleBar>
	{
	public:
		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;

		void Sizing(RECT newRect) override final;

		TitleBar();

		TitleBar(wstring text, RECT rc);

		TitleBar(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);

		~TitleBar();
	};

	///<EditBoxInterface>编辑框接口</EditBoxInterface>
	class EditBoxInterface : public Control
	{
		wstring mFrontName;

	public:

		void ChangeFrontName(wstring newFrontName);

		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;

		EditBoxInterface();

		EditBoxInterface(RECT rc, string defaultText = "");

		EditBoxInterface(wstring defaultText, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor);
	};

	class EditBox :public EditBoxInterface,
		public REFLECT<EditBox> {
	public:
		EditBox();
	};

	class ReadOnlyEditBox : public EditBoxInterface,
		public REFLECT<ReadOnlyEditBox>{
	public:
		ReadOnlyEditBox();
	};

	class NoCaretEditBox :public EditBoxInterface,
		public REFLECT<NoCaretEditBox> {
	public:
		NoCaretEditBox();
	};

	///<LabelBoxInterface>标签接口</LabelBoxInterface>
	class LabelBoxInterface :public Control
	{
	public:
		LabelBoxInterface();

		void Draw(Listener * pListener) override;

		UINT HitTest(Listener * pListener, POINT pt) override;
	};

	class StaticLabel : public LabelBoxInterface,
		public REFLECT<StaticLabel> 
	{
	public:
		StaticLabel();

		void Draw(Listener * pListener) override;
	};

	class LabelBox : public LabelBoxInterface,
		public REFLECT<LabelBox>{
	public:
		LabelBox();
	};

	class DrawAbleLabel : public LabelBoxInterface,
		public REFLECT<DrawAbleLabel>
	{
		int mSubID;
	public:
		DrawAbleLabel();

		~DrawAbleLabel();

		wstring  mStateType;

		POINT mStartDrawPoint;

		POINT mEndDrawPoint;

		list<Control *> mDrawSet;

		Control * pBufferControl;

		void Draw(Listener * pListener) override;

		UINT LButtonDown(Listener * pListener, POINT pt) override;

		UINT LButtonUp(Listener * pListener, POINT pt) override;

		void MouseMove(Listener * pListener, POINT pt) override;
	};

	class ForceLabel : public LabelBoxInterface,
		public REFLECT<ForceLabel>
	{
		bool mDrawForce;
	public:
		ForceLabel();

		void Draw(Listener * pListener) override;
	};

	///<ComposeControl>组合控件</ComposeControl>
	class ComposeControl :public Control
	{
	protected:

		list<ComposeControl *>  mControl; 

		Listener * pmListener;

	public:
		bool Attach(Listener * pListener);

		bool Add(Control * pControl);

		virtual void Draw(Listener * pListener);

		virtual UINT HitTest(POINT pt);

		virtual void Hover(POINT pt);
	};

	template<class CellType>
	class Cell
	{
	protected:
		CellType mCellHeight;
	public:
		Cell() :mCellHeight((CellType)10) {}
		CellType getCellHeight() { return mCellHeight; }
		void setCellHeight(CellType ht) { mCellHeight = ht; }
	};

	class ListInterface:Control,
		public Cell<int>
	{
	protected:
		map<string, string> mValueMap;
	public:
		ListInterface();

		void Draw(Listener * pListener) override;
	};
}



