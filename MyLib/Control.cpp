#include "Control.h"
#include "MyLayout.h"

namespace LIB_CONTROL
{
	using namespace Conver;

	using namespace MutexLock;

	bool DragAdapter::CanDrag()
	{
		return mCanDrag;
	}

	void DragAdapter::Stop()
	{
		mbDraging = false;
	}

	void DragAdapter::SetDrag(bool DragState)
	{
		mCanDrag = DragState;
	}

	DragAdapter::DragAdapter():mCanDrag(false),mbDraging(false)
	{
	}

	Control::~Control()
	{
	}

	UINT Listener::ListenedWidth()
	{
		IS_RETURN(!mListenedWindow,0);
		RECT rc;
		GetWindowRect(mListenedWindow, &rc);
		return RECTWIDTH(rc);
	}

	UINT Listener::ListenedHeight()
	{
		IS_RETURN(!mListenedWindow, 0);
		RECT rc;
		GetWindowRect(mListenedWindow, &rc);
		return RECTHEIGHT(rc);
	}

	Control * Listener::findElementByID(UINT id)
	{
		 auto tempControl = find_if(mpControl.begin(), mpControl.end(), [&id](Control * pControl)->bool
		 {
			 if (id == pControl->getID()) return true;
			 return false;
		 });
		 if (tempControl == mpControl.end()) return nullptr;
		 return *tempControl;
	}

	const Control Listener::findElementByName(const wstring &strName)
	{
		auto pFindControl=find_if(mpControl.begin(), mpControl.end(), [&strName](Control * pControl)->bool {
				return (strName == pControl->NickName()) ? true : false;
			});
		if (pFindControl == mpControl.end()) return *Control::EMPTY;
		return **pFindControl;
	}

	bool Listener::AddClickFuncByID(UINT id,function<void(void)> pFunc,bool rewrite)
	{
		auto pControl = findElementByID(id);
		IS_RETURN_ERROR(!pControl,false,"AddClickFuncByID找不到指定的控件");
		IS_RETURN(!rewrite && pControl->mClickFunc != nullptr, false);
		pControl->mClickFunc = pFunc;
		return true;
	}

	bool Listener::AddClickFuncByName(const wstring & strName, function<void(void)> pFunc, bool rewrite)
	{
		auto pControl = findElementByName(strName);
		IS_RETURN_ERROR(&pControl==Control::EMPTY, false, "AddClickFuncByName找不到指定的控件");
		IS_RETURN(!rewrite && pControl.mClickFunc != nullptr, false);
		pControl.mClickFunc = pFunc;
		return true;
	}

	bool Listener::SetValueByID(UINT id, wstring stringValue)
	{
		auto pControl = findElementByID(id);
		IS_RETURN_ERROR(!pControl, false, "SetValueByID找不到指定的控件");
		pControl->SetText(stringValue);
		return true;
	}

	wstring Listener::getValueByID(UINT id)
	{
		auto pControl = findElementByID(id);
		IS_RETURN_ERROR(!pControl, L"", "getValueByID找不到指定的控件");
		return pControl->Text();
	}

	void Listener::SetRangeIDValue(UINT startId, UINT endId, wstring stringValue)
	{
		if (endId < startId) return;
		for (auto id = startId; id <= endId; id++)
		{
			SetValueByID(id,stringValue);
		}
	}

	bool Listener::attachWindow(HWND hWnd)
	{
		IS_RETURN_ERROR(!hWnd,false,"attachWindow 失败! 句柄为空");
		mListenedWindow = hWnd;
		return true;
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
		auto tempIt = *it;
		mpControl.erase(it);
		SAFE_DELETE(tempIt);
		return true;
	}

	void Listener::Draw()
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->Draw(this);
			pControl->DrawBoard(this);
		});
		//render caret
		CaretManager.Render();
	}

	void Listener::InputChar(wchar_t c)
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			if(pControl->HaveCaret()) pControl->InputChar(this, c);
		});
	}

	void Listener::Hover(POINT pt)
	{ 
		Control * pTempControl = NULL;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->SetInternal(false);
			if (PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				pTempControl = pControl;
				pControl->SetInternal(true);
			}
			else
			{
				//把鼠标之外的所有控件设置为没有鼠标形状
				if (pControl->mbHaveShaper)
					pControl->mbHaveShaper = false;
			}
			pControl->Hover(this, pt);
		});
	}
		
	UINT Listener::LButtonDown(POINT pt)
	{
		UINT ret = SHOULD_DO_NOTHING;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->KillFocus(this);  //KillFocus
			if (PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				if (pControl->mClickFunc) pControl->mClickFunc(); //调用点击事件
				pControl->Focus(this); //SetFocus
				ret = pControl->LButtonDown(this, pt);
				if (pControl->IsDisable()) ret = SHOULD_DO_NOTHING;
			}
			return ret;
		});

		return ret;
	}

	UINT Listener::LButtonUp(POINT pt)
	{
		UINT ret = SHOULD_DO_NOTHING;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->Stop();//Drag End
			if (PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				ret = pControl->LButtonUp(this, pt);
				if (pControl->IsDisable()) { ret = SHOULD_DO_NOTHING; }
			}
			return ret;
		});
		return ret;
	}

	void Listener::MouseMove(POINT pt)
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * ItControl) {
			if (ItControl->IsMouseInteral()) ItControl->MouseMove(this, pt);
		});
		return;
	}

	UINT Listener::HitTest( POINT pt)
	{
		Control * pTempControl = NULL;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			if (PointInRect(pt.x, pt.y, pControl->getRect()))
			{
				pTempControl = pControl;
			}
		});
		if(!pTempControl || pTempControl->IsDisable())  return HTCLIENT;
		return pTempControl->HitTest(this, pt);
	}

	void Listener::ChangeSize(RECT newRect)
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			pControl->Sizing(newRect);
		});
	}

	list<Control*>& Listener::Obj()
	{
		return mpControl;
	}

	Control * Control::EMPTY = new Control(true);

	void Control::SetClassName(wstring className)
	{
		mClassName = className;
	}

	wstring Control::getClassName()
	{
		return mClassName;
	}

	UINT Control::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	void Control::Draw(Listener * pListener)
	{
		//默认画背景
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect,mBackColor,true);
		}
		else
		{
			DrawManager.DrawRectangle(mRect,mHonverBackColor,true);
		}
	}

	void Control::Hover(Listener * pListener, POINT pt)
	{
		if (!mOwnCaret)
		{
			if (mMouseInternal)
			{
				if (!mbHaveShaper) SetCursor(ArrowShape::SHAPE_ARROW);
				mbHaveShaper = true;
			}
			return;
		};
		if (mMouseInternal)
		{
			if (!mbHaveShaper)
			{
				SetCursor(ArrowShape::SHAPE_I);
				mbHaveShaper = true;
			}
		}
	}

	UINT Control::LButtonDown(Listener * pListener,POINT pt)
	{
		mBDownInternal = true;
		mbDraging = true;
		mouseDragStartPoint = pt;

		if (!mOwnCaret)   return 0;
		if (!mFocusCaret) return 0;
		CaretManager.ChangeCaretSize(0, height());
		CaretManager.ShowCaret();
		if (mText.empty()) SAFE_RELEASE(mpTextpLayout);
		CaretManager.AdjustPos(mRect,mpTextpLayout,&pt);
		return 0; 
	}

	UINT Control::LButtonUp(Listener * pListener,POINT pt)
	{
		mbDraging = false;
		if (!mOwnCaret) return 0;
		CaretManager.ShowCaret();
		CaretManager.AdjustPos(mRect,mpTextpLayout);
		return 0;
	}

	void Control::MouseMove(Listener * pListener,POINT pt)
	{
		if (!mMouseInternal) return;
		if (!mbDraging) return;
		CaretManager.HideCaret(); ///隐藏光标
		Drag(pListener,pt.x-mouseDragStartPoint.x,pt.y-mouseDragStartPoint.y);
		mouseDragStartPoint = pt;
	}

	void Control::InputChar(Listener * pListener, wchar_t cUnicode)
	{
		//输入字符事件处理
		if (!mOwnCaret) return;
		if (mReadOnly) return; //只读
		if (!mFocusCaret) return;

		auto index = CaretManager.getIndex();
		auto CaretPosition = CaretManager.pos();
		Point tempPoint; 

		CaretManager.isHead = false;
		if ( index == -1 ) { index = 0; CaretManager.isHead = true; }
		if ( index!= 0 &&index > STCAST(int,mText.size() - 1)) index = mText.size() - 1;
		if (cUnicode == '\b') 
		{
			if (index == 0 && mText.empty()) return;
			mText.erase(index,1); 	
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
			CaretManager.DecIndex();
		}
		else if (cUnicode == VK_LEFT)
		{
			if (index == 0) {
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
				CaretManager.DecIndex();
				return;
			}
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
			CaretManager.DecIndex();
			tempPoint = CaretManager.pos();
			if (tempPoint == CaretPosition)
			{
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex(), false);
				CaretManager.DecIndex(); // "后置"
			}
		}
		else if (cUnicode == VK_RIGHT)
		{
			if (index == mText.size() - 1) {
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, true);
				return;
			}
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, true);
			CaretManager.IncIndex();
			tempPoint = CaretManager.pos();				   
			if (tempPoint == CaretPosition)
			{
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex(), true);
			}
		}
		else
		{
 			auto posInsert = index + 1;
			if (CaretManager.isHead) { posInsert = 0; CaretManager.isHead = false; }
			//添加字符
			if (mText.empty()) 
				mText = cUnicode;
			else
			{
				mText = mText.insert(posInsert, wstring(&cUnicode));
			}
			Draw(pListener);
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, posInsert, true);
			CaretManager.IncIndex();
		}
	}

	void Control::Drag(Listener * pListener, int dx, int dy)
	{
		if (!mCanDrag) return;
		auto rWidth = width(); auto rHeight = height();
		mRect = RectOffSet(mRect,dx,dy,dx,dy);
	}

	void Control::Focus(Listener * pListener)
	{
		mFocusCaret = true;
	}

	void Control::KillFocus(Listener * pListener)
	{
		mFocusCaret = false;
	}

	void Control::Sizing(RECT newRect)
	{

	}

	void Control::DrawBoard(Listener * pListener)
	{
		if (!mHaveBoard) return;
		DrawManager.DrawRectangle(mRect,COLOREX(mBoardColor),false,mBoardSize);
	}

	bool Control::HaveCaret()
	{
		return mOwnCaret;
	}

	void Control::SetCaretState(bool state)
	{
		mOwnCaret = state;
	}

	bool Control::Stretch()
	{
		return mCanStretch;
	}


	bool Control::Active() const
	{
		return mActive;
	}

	void Control::SetActive(bool bActive)
	{
		mActive = bActive;
	}

	wstring Control::Text() const
	{
		return mText;
	}

	void Control::SetText(wstring text)
	{
		if (!text.empty()) mText = text;
	}

	ColorStruct Control::ForceColor()
	{
		 return ColorStruct(GetRValue(mForceColor),
			GetGValue(mForceColor), GetBValue(mForceColor));
	}

	void Control::SetForceColor(COLORREF color)
	{
		mForceColor = color;
	}

	ColorStruct Control::BackColor()
	{
		return ColorStruct(GetRValue(mBackColor),
			GetGValue(mBackColor), GetBValue(mBackColor));
	}

	void Control::SetBackColor(COLORREF color)
	{
		mBackColor = color;
	}

	ColorStruct Control::HoverBackColor()
	{
		return ColorStruct(GetRValue(mHonverBackColor),
			GetGValue(mHonverBackColor), GetBValue(mHonverBackColor));
	}

	void Control::SetHoverBackColor(COLORREF color)
	{
		mHonverBackColor = color;
	}

	ColorStruct Control::HoverForceColor()
	{
		return ColorStruct(GetRValue(mHoverForceColor),
			GetGValue(mHoverForceColor), GetBValue(mHoverForceColor));
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

	RECT & Control::getRect() 
	{
		return mRect;
	}

	void Control::AdjustRect(RECT decREct)
	{
		mRect = decREct;
	}

	bool Control::IsMouseInteral() const
	{	
		return mMouseInternal?true:false;
	}

	void Control::SetInternal(bool MouseInternal)
	{
		mMouseInternal = MouseInternal;
	}


	LONG Control::width() const
	{
		return RECTWIDTH(mRect);
	}

	void Control::SetWidth(UINT uWidth)
	{
		mRect.right = mRect.left+ uWidth;
	}

	LONG Control::height() const
	{
		return RECTHEIGHT(mRect);
	}

	void Control::SetHeight(UINT uHeight)
	{
		mRect.bottom = mRect.top + uHeight;
	}

	Control::Control(wstring text, RECT rc):mRect(rc),mText(text),mVisible(true),mBackColor(RGB(45,45,48)),
		mForceColor(RGB(110,110,112)), mHonverBackColor(RGB(63, 63, 65)),mHoverForceColor(RGB(110, 110, 112)),
		mCanStretch(false),mBDownInternal(false),mOwnCaret(false),mpTextpLayout(nullptr),mFocusCaret(false),mbHaveShaper(false),mNickName(L"null")
	{
	}

	Control::Control():Control(L"",nullRect)
	{

	}

	Control::Control(bool bEmpty):mEmpty(bEmpty){}

	Control::Control(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,rc)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
		SetHoverBackColor(hoverBackColor);
		SetHoverForceColor(hoverForceColor);
	}


	bool ImageAdapter::SetImage(IPIC * img)
	{
		if (!img) return false;
		pImage = img; return true;
	}

	bool ImageAdapter::LoadFromFile(wstring strFileName)
	{
		return SetImage(DrawManager.CreateBitmap(const_cast<wchar_t *>(strFileName.c_str())));
	}

	void ImageAdapter::SetImgRect(RECT desRect)
	{
		mImgRec = desRect;
	}

	RECT ImageAdapter::ImgRect() const
	{
		return mImgRec;
	}

	ImageAdapter::ImageAdapter() :pImage(nullptr), bDrawImage(false)
	{
		mImgRec = {0,0,0,0};
	}

	ImageAdapter::~ImageAdapter()
	{
		SAFE_RELEASE(pImage);
	}

	void TitleBar::Draw(Listener * pListener)
	{
		if (!mMouseInternal)
		{
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor), &mpTextpLayout, true,DWRITE_TEXT_ALIGNMENT_LEADING);
			IS_ERROR_EXIT(!ret, "Draw TitleBar failed!");
		}
		else
		{
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor), &mpTextpLayout, true, DWRITE_TEXT_ALIGNMENT_LEADING);
			IS_ERROR_EXIT(!ret, "Draw  honvered TitleBar failed!");
		}
	}


	UINT TitleBar::HitTest(Listener * pListener, POINT pt)
	{
		return HTCAPTION;
	}

	void TitleBar::Sizing(RECT newRect)
	{
		auto titleBarHeight = height();
		//AdjustRect(MyRect(0, 0, RECTWIDTH(newRect)-3*titleBarHeight, titleBarHeight));
	}

	TitleBar::TitleBar()
	{
		SetID(CONTROL_TYPE_TITLEBAR);
	}

	TitleBar::TitleBar(wstring text,RECT rc):Control(text,rc)
	{
		SetID(CONTROL_TYPE_TITLEBAR);
	}

	TitleBar::TitleBar(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) 
	{
	}

	TitleBar::~TitleBar()
	{

	}
////////////////////////////////////////////////////////////////////////// ButtonInterface
	void ButtonInterface::SetButtonDownInternal(bool isDownInternal)
	{
		mBDownInternal = isDownInternal;
	}

	void ButtonInterface::SetBoardColor(COLORREF cBoardColor)
	{
		mBoardColor = cBoardColor;
	}

	void ButtonInterface::NeedDrawBoard(bool bDrawBoard)
	{
		mBoardColor = bDrawBoard;
	}

	void ButtonInterface::Draw(Listener * pListener)
	{
		if (!mMouseInternal)
		{
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor), &mpTextpLayout, true);
			IS_ERROR_EXIT(!ret, "Draw Button failed!");
		}
		else
		{
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor), &mpTextpLayout, true);
			IS_ERROR_EXIT(!ret, "Draw  honvered Button failed!");
		}
	}

	UINT ButtonInterface::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	ButtonInterface::ButtonInterface()
	{
		mDrawBoard = false;
	}

	ButtonInterface::ButtonInterface(wstring text, RECT rc) :Control(text, rc), mBoardColor(mBackColor), mDrawBoard(false)
	{
	}

	ButtonInterface::ButtonInterface(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor) : Control(text,
		rc, forceColor, backColor, hoverForceColor, hoverBackColor), mBoardColor(mBackColor), mDrawBoard(false)
	{
	}

	Button::Button()
	{
		SetID(CONTROL_TYPE_DEFAULT_BUTTON);
		
	}

	CloseButton::CloseButton()
	{
		SetID(CONTROL_TYPE_CLOSE_BUTTON);
	}

	CloseButton::CloseButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):ButtonInterface(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		CloseButton();
	}

	void CloseButton::Draw(Listener * pListener)
	{
		auto dx = (UINT)(2.0 / 3 * width());
		auto dy = (UINT)(2.0 / 3 * height());
		RECT drawRect = Conver::ClipRectBoard(mRect, dx, dy);

		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawLine(LeftTopPoint(drawRect), RightBottomPoint(drawRect), COLOREX(mForceColor));
			DrawManager.DrawLine(LeftBottomPoint(drawRect), RightTopPoint(drawRect), COLOREX(mForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawLine(LeftTopPoint(drawRect), RightBottomPoint(drawRect), COLOREX(mHoverForceColor));
			DrawManager.DrawLine(LeftBottomPoint(drawRect), RightTopPoint(drawRect), COLOREX(mHoverForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}


	UINT CloseButton::LButtonUp(Listener * pListener,POINT pt)
	{
		if (mBDownInternal) return SHOULD_CLOSE_WINDOW;
		return SHOULD_DO_NOTHING;
	}

	UINT CloseButton::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	void CloseButton::Sizing(RECT newRect)
	{
		auto btnHeight = height();
		AdjustRect(MyRect(RECTWIDTH(newRect)-btnHeight,0, RECTWIDTH(newRect), btnHeight));
	}

	MiniButton::MiniButton()
	{
		SetID(CONTROL_TYPE_MINI_BUTTON);
	}

	MiniButton::MiniButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):ButtonInterface(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		MiniButton();
	}

	void MiniButton::Draw(Listener * pListener)
	{
		auto dx = (UINT)((1.0 / 3)*width());
		auto dy = (UINT)((2.0 / 3)*height());
		RECT drawRect = ClipRectBoard(mRect, dx, dy);
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawLine(LeftTopPoint(drawRect),RightTopPoint(drawRect), COLOREX(mForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawLine(LeftTopPoint(drawRect),RightTopPoint(drawRect), COLOREX(mHoverForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}

	UINT MiniButton::LButtonUp(Listener * pListener,POINT pt)
	{
		if (mBDownInternal) return SHOULD_MINI_WINDOW;
		return SHOULD_DO_NOTHING;
	}

	void MiniButton::Sizing(RECT newRect)
	{
		auto btnHeight = height();
		AdjustRect(MyRect(RECTWIDTH(newRect) - 3*btnHeight, 0, RECTWIDTH(newRect)-2*btnHeight, btnHeight));
	}

	void MaxButton::DrawMaxImg()
	{
		auto dx = (UINT)(2.0 / 3 * width());
		auto dy = (UINT)(2.0 / 3 * height());
		RECT drawRect = ClipRectBoard(mRect, dx, dy);
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawRectangle(drawRect, COLOREX(mForceColor), false);
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawRectangle(drawRect, COLOREX(mHoverForceColor), false);
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}

	void MaxButton::DrawRestoreImg()
	{
		auto dx = (UINT)(2.0 / 3 * width());
		auto dy = (UINT)(2.0 / 3 * height());
		RECT drawRect = ClipRectBoard(mRect, dx, dy);
		auto centerPt = CenterPoint(drawRect);
		dx = (UINT)(1.0 / 3 * width());
		dy = (UINT)(1.0 / 3 * height());
		RECT rightTopRect = { centerPt.x , centerPt.y - dy, centerPt.x + dx,centerPt.y };
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawRectangle(rightTopRect, COLOREX(mForceColor), false);
			DrawManager.DrawRectangle(drawRect, COLOREX(mForceColor), false);
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawRectangle(rightTopRect, COLOREX(mHoverForceColor), false);
			DrawManager.DrawRectangle(drawRect, COLOREX(mHoverForceColor), false);
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}

	void MaxButton::setState(bool isMax)
	{
		this->isMax = isMax;
	}

	MaxButton::MaxButton()
	{
		SetID(CONTROL_TYPE_MAXI_BUTTON);
		isMax = false;
	}

	MaxButton::MaxButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):ButtonInterface(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		MaxButton();
		isMax = false;
	}

	void MaxButton::Draw(Listener * pListener)
	{
		if (!isMax)
		{
			DrawMaxImg();
		}
		else DrawRestoreImg();
	}

	UINT MaxButton::LButtonUp(Listener * pListener,POINT pt)
	{
		if (mBDownInternal) 
		{
			if (isMax) 
			{
				isMax = !isMax;
				return SHOULD_RESTORE_WINDOW;
			}	
			else 
			{
				isMax = !isMax;
				return SHOULD_MAX_WINDOW;
			}
		}
		return SHOULD_DO_NOTHING;
	}

	void MaxButton::Sizing(RECT newRect)
	{
		auto btnHeight = height();
		AdjustRect(MyRect(RECTWIDTH(newRect) - 2*btnHeight, 0, RECTWIDTH(newRect) - btnHeight, btnHeight));
	}

	ImageButton::ImageButton()
	{
		pHonverImage = nullptr;
		mBackColor = RGB(0,0,0);
		mHonverBackColor = RGB(0,0,0);
	}

	ImageButton::ImageButton(wstring picLoc, RECT rImgRect)
	{
		pImage = DrawManager.CreateBitmap(const_cast<wchar_t *>(picLoc.c_str()));
		mRect = rImgRect;
	}

	ImageButton::ImageButton(IPIC * pBitmap, RECT rImgRect)
	{
		pImage = pBitmap;
		mRect = rImgRect;
	}

	ImageButton::~ImageButton()
	{
		SAFE_RELEASE(pHonverImage);
	}

	void ImageButton::Draw(Listener * pListener)
	{
		static RECT oldRect = mRect;
		if (!pImage) if (!mText.empty()) LoadFromFile(mText); //如果文字非空，那么就加载文字路径下的图
		if (pHonverImage == nullptr) pHonverImage = pImage;
		if (ZeroRect(mImgRec)) mImgRec = mRect;
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect,COLOREX(mBackColor), true); 
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
		}
		if (pImage) DrawManager.DrawPicture(pImage, mRect);
	}

	////////////////////////////////////////////////////////////////////////// EditBox
	EditBoxInterface::EditBoxInterface(RECT rc,string defaultText)
	{
		mForceColor = mHoverForceColor = RGB(255,255,255);
		mBackColor = mBackColor = RGB(116,116,119);
	}

	EditBoxInterface::EditBoxInterface()
	{
		mOwnCaret = true;
	}

	EditBoxInterface::EditBoxInterface(wstring defaultText, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor) :Control(defaultText,
		rc, forceColor, backColor, hoverForceColor, hoverBackColor) {
		mOwnCaret = true; //must have caret
	}

	void EditBoxInterface::ChangeFrontName(wstring newFrontName)
	{
		mFrontName = newFrontName;
	}

	void EditBoxInterface::Draw(Listener * pListener)
	{
		DrawManager.DrawRectangle(mRect,COLOREX(mBackColor),true);
		DrawManager.DrawRectangle(mRect,mBoardColor, false); //draw board
		DrawManager.DrawTextW(mText, mRect, mForceColor, STCAST(float, RECTHEIGHT(mRect) - ALIGN_UPDPWNDISTANCE), &mpTextpLayout,ALIGN_TEXT_LEFT,ALIGN_PARAGRAPH_CENTER);
	}
	
	UINT EditBoxInterface::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	////////////////////////////////////////////////////////////////////////// ComposeControl
	bool ComposeControl::Attach(Listener * pListener)
	{
		pmListener = pListener;
		return false;
	}

	bool ComposeControl::Add(Control * pControl)
	{
		auto it = std::find_if(mControl.begin(), mControl.end(), [&pControl](Control * ItControl) {
			if (ItControl == pControl) return false;
			return true;
		});
		if (it != mControl.end()) return false;
		return true;
	}

	void ComposeControl::Draw(Listener * pListener)
	{
		for_each(mControl.begin(), mControl.end(), [&](Control * ItControl) {
			ItControl->Draw(pmListener);
		});
	}

	UINT ComposeControl::HitTest(POINT pt)
	{
		for_each(mControl.begin(), mControl.end(), [&](Control * ItControl) {
			ItControl->HitTest(pmListener, pt);
		});
		return 0;
	}

	void ComposeControl::Hover(POINT pt)
	{
		for_each(mControl.begin(), mControl.end(), [&](Control * ItControl) {
			ItControl->Hover(pmListener, pt);
		});
	}


////////////////////////////////////////////////////////////////////////// LabelBoxInterface
	LabelBoxInterface::LabelBoxInterface() {}


	void LabelBoxInterface::Draw(Listener * pListener)
	{
		Control::Draw(pListener);
		DrawManager.DrawTextW(mText, mRect, COLOREX(mForceColor), STCAST(float,RECTHEIGHT(mRect)-ALIGN_UPDPWNDISTANCE),&mpTextpLayout);
	}

	UINT LabelBoxInterface::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	bool ReadAble::isReadOnly()
	{
		return mReadOnly;
	}

	void ReadAble::setReadAccess(bool readAccess)
	{
		mReadOnly = readAccess;
	}

	ReadAble::ReadAble():mReadOnly(true)
	{
	}

	ReadOnlyEditBox::ReadOnlyEditBox(){}

	EditBox::EditBox()
	{
		mReadOnly = false;
	}

	NoCaretEditBox::NoCaretEditBox()
	{
		mOwnCaret = false;
	}

	LabelBox::LabelBox(){}

	DrawAbleLabel::DrawAbleLabel()
	{
		pBufferControl = pForcedControl =  nullptr;
		mStartDrawPoint = nullPoint;
		mSubID = DEFAULT_CONTROL_ID;
	}

	DrawAbleLabel::~DrawAbleLabel()
	{
		SAFE_DELETE_ALL(mDrawSet);
		SAFE_DELETE_ALL(mSaveSet);																	 
	}

	bool DrawAbleLabel::SaveControl(float widthSacle, float heightSacle,Control * pControl)
	{
		IS_RETURN(!pControl,false);
		auto internalRect = pControl->getRect();
		auto itControl = 
		find_if(mSaveSet.begin(), mSaveSet.end(), [&pControl](Control * pSaveContorl) {
			if (pSaveContorl->getID() == pControl->getID())	return true;
			return false;
		});
		Control * newControl = (itControl == mSaveSet.end())?new Control(*pControl):*itControl;

		newControl->AdjustRect(
			MyRect(STCAST(INT,(internalRect.left - mRect.left)*widthSacle),
				STCAST(INT,(internalRect.top - mRect.top)*heightSacle),
				STCAST(INT,(internalRect.right - mRect.left)*widthSacle),
				STCAST(INT,(internalRect.bottom - mRect.top)*heightSacle))
		);

		newControl->Enable();
		newControl->SetDrag(false);
		
		if(itControl == mSaveSet.end())
		mSaveSet.push_back(newControl);

		if (find_if(mSaveSet.begin(), mSaveSet.end(), [&pControl](Control * pSaveContorl) {
			if (pSaveContorl->getID() == 0)	return true;
			return false;
		}) == mSaveSet.end())
		{
			//背景
			BackGround * pForm = new BackGround();
			pForm->AdjustRect(MyRect(0, 0, RECTWIDTH(mRect), RECTHEIGHT(mRect)));
			pForm->SetClassName(L"BackGround");
			pForm->SetBackColor(mBackColor);
			mSaveSet.push_front(pForm);
		}
		return true;
	}

	bool DrawAbleLabel::SaveRangleControlByID(UINT uStartId, UINT uEndId, Listener * pListener)
	{
		for (auto id = uStartId; id <=uEndId; id++)
		{
			SaveControl(1, 1, pListener->findElementByID(id));
		}
		return false;
	}

	bool DrawAbleLabel::LoadControl(string strFileName)
	{
		Listener pTempListener;
		Layout::ControlLayout.LoadLayoutFile(strFileName.empty() ? DEFAULT_LAYOUT : strFileName.c_str(), &pTempListener);
		mSaveSet = pTempListener.Obj();
		mDrawSet = pTempListener.Obj();
		for_each(mSaveSet.begin(), mSaveSet.end(), [&](Control * pControl) {

			auto internalRect = pControl->getRect();
			pControl->AdjustRect(
				MyRect((internalRect.left + mRect.left),
				(internalRect.top + mRect.top),
					(internalRect.right + mRect.left),
					(internalRect.bottom + mRect.top))
			);
		});
		return false;
	}

	bool DrawAbleLabel::UpdateRectByID(UINT id, int iWidht, int iHeight)
	{
		return true;
	}

	void DrawAbleLabel::Draw(Listener * pListener)
	{
		__super::Draw(pListener);
		if (mStateType.empty()) return;
	}

	UINT DrawAbleLabel::LButtonDown(Listener * pListener, POINT pt)
	{
		IS_RETURN(mStateType.empty(),0);
		mStartDrawPoint = pt;
		return 0;
	}

	UINT DrawAbleLabel::LButtonUp(Listener * pListener, POINT pt)
	{
		IS_RETURN(mStateType.empty(), 0);
		if (pBufferControl != nullptr)
		{
			pListener->detach(pBufferControl);
		}
		mEndDrawPoint = pt;

		Control * pControl = MyFactory.create<Control>(WCharToAChar(COCAST(wchar_t *, mStateType.c_str())));
		IS_RETURN_ERROR(!pControl,0,"LButtonUp:: MyFactory.create failed..");
		pControl->AdjustRect(MyRect(mStartDrawPoint,mEndDrawPoint));
		pControl->SetClassName(mStateType);
		pControl->SetID(mSubID++);
		//全部可以移动
		pControl->SetDrag(true);
		pControl->Disable();
		mDrawSet.push_back(pControl);
		pListener->attach(pControl);
		//保存另一份
		SaveControl(1, 1, pControl);
		mStateType.clear();
		mStartDrawPoint = nullPoint;
		pBufferControl = nullptr;
		return 0;
	}

	void DrawAbleLabel::MouseMove(Listener * pListener, POINT pt)
	{
		if (mStateType.empty())
		{
			auto forcedIt = find_if(mDrawSet.rbegin(), mDrawSet.rend(), [&](Control * pControl)
			{
				if (PointInRect(pt.x, pt.y, pControl->getRect())) return true;
				return false;
			});
			if (forcedIt == mDrawSet.rend()) { return ; }
			pForcedControl = *forcedIt;
			return ;
		}

		static Point lastPt = pt;
		if (mStateType.empty()||  nullPoint == mStartDrawPoint) return;
		if (lastPt == pt) return;
		if (pBufferControl != nullptr)
		{
			pListener->detach(pBufferControl);
		}
		pBufferControl = MyFactory.create<Control>(WCharToAChar(COCAST(wchar_t *, mStateType.c_str())));
		IS_RETURN_ERROR(!pBufferControl, , "MouseMove:: MyFactory.create failed..");
		pBufferControl->AdjustRect(MyRect(mStartDrawPoint, pt));
		mEndDrawPoint = pt;
		pListener->attach(pBufferControl);
		lastPt = pt;
	}

	ForceLabel::ForceLabel() {}

	void ForceLabel::Draw(Listener * pListener)
	{
		if (!mFocusCaret)
		{
			DrawManager.DrawRectWithTextW(mRect,mText, COLOREX(mBackColor), COLOREX(mForceColor),&mpTextpLayout,true,ALIGN_TEXT_LEFT);
		}
		else
		{
			DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor), &mpTextpLayout,true, ALIGN_TEXT_LEFT);
		}
	}

	StaticLabel::StaticLabel()
	{
	}

	void StaticLabel::Draw(Listener * pListener)
	{
		if (IsMouseInteral())
		{
			DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor), &mpTextpLayout, true, ALIGN_TEXT_LEFT);
		}
		else
		{
			DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor), &mpTextpLayout, true, ALIGN_TEXT_LEFT);
		}
	}

	bool DisableAdapter::IsDisable()
	{
		return mDisable;
	}

	DisableAdapter::DisableAdapter():mDisable(false){}

	void DisableAdapter::Disable()
	{
		mDisable = true;
	}

	void DisableAdapter::Enable()
	{
		mDisable = false;
	}

	BackGround::BackGround()
	{
		SetID(0);
	}

	void BackGround::Draw(Listener * pListener)
	{
		DrawManager.DrawRectangle(mRect, COLOREX(mBackColor),true);
	}

}
