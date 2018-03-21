#include "Control.h"

namespace LIB_CONTROL
{
	using namespace Conver;

	bool DragAdapter::IsDraging()
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

	DragAdapter::DragAdapter():mCanDrag(false)
	{
	}

	Control::~Control()
	{
	}

	bool Listener::attachWindow(HWND hWnd)
	{
		IS_RETURN_ERROR(!hWnd,false,"attachWindow Ê§°Ü! ¾ä±úÎª¿Õ");
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
				pControl->Focus(this, mListenedWindow); //SetFocus
				if (pControl->LButtonDown(this, pt) != SHOULD_DO_NOTHING) return ret;
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
				ret = pControl->LButtonUp(this,pt);
				if (ret != SHOULD_DO_NOTHING) return ret; 
				return ret;
		});
		return ret;
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
		if(!pTempControl)  return HTCLIENT;
		return pTempControl->HitTest(this, pt);
	}

	void Listener::MouseMove(POINT pt)
	{
		for_each(mpControl.begin(), mpControl.end(), [&](Control * ItControl) {
			if (ItControl->IsMouseInteral()) ItControl->MouseMove(this,pt);
		});
		return;
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

	void Control::SetID(UINT typeId)
	{
		mControlTypeId = typeId;
	}

	UINT Control::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	void Control::Draw(Listener * pListener)
	{
		//Ä¬ÈÏ»­±³¾°
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
		if (!mOwnCaret) return;
		if (mMouseInternal) SetCursor(ARROW::ArrowShape::SHAPE_I);
		else SetCursor(ARROW::ArrowShape::SHAPE_ARROW);
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
		IS_RETURN_ERROR(!mpTextpLayout,0,"Control::LButtonDown mpTextLayout null...");
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
		CaretManager.HideCaret(); ///Òþ²Ø¹â±ê
		Drag(pListener,pt.x-mouseDragStartPoint.x,pt.y-mouseDragStartPoint.y);
		mouseDragStartPoint = pt;
	}

	void Control::InputChar(Listener * pListener, wchar_t cUnicode)
	{
		if (!mOwnCaret) return;
		if (!mpTextpLayout) return;
		if (!mFocusCaret) return;

		if (cUnicode == '\b') 
		{
			if (mText.size() <= 0) return;
			auto index = CaretManager.getIndex();
			mText.erase(index,1); 
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex(), false);
			CaretManager.DecIndex();
		}
		else
		{
			if (mText.size() == 0) mText = wstring(&cUnicode);
			else mText = mText.insert(CaretManager.getIndex() + 1, wstring(&cUnicode));
			Draw(pListener);//must draw to find next pos
			CaretManager.IncIndex();
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex() + 1, true);
		}
	}

	void Control::Drag(Listener * pListener, int dx, int dy)
	{
		if (!mCanDrag) return;
		auto rWidth = width(); auto rHeight = height();
		mRect = RectOffSet(mRect,dx,dy,dx,dy);
	}

	void Control::Focus(Listener * pListener, HWND hWnd)
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

	UINT Control::TypeId() const
	{
		return mControlTypeId;
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

	void Control::SetBoardColor(COLORREF color)
	{
		mBoardColor = color;
	}

	COLORREF Control::getBoardColor()
	{
		return mBoardColor;
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
		return mRect.right - mRect.left;
	}

	LONG Control::height() const
	{
		return mRect.bottom - mRect.top;
	}

	Control::Control(wstring text, RECT rc):mRect(rc),mText(text),mVisible(true),mBackColor(RGB(45,45,48)),
		mForceColor(RGB(110,110,112)), mHonverBackColor(RGB(63, 63, 65)),mHoverForceColor(RGB(110, 110, 112)),
		mCanStretch(false),mBDownInternal(false),mOwnCaret(false),mpTextpLayout(nullptr),mBoardColor(RGB(255,255,255))
	{
		mbDraging = false;
	}

	Control::Control() :Control(L"")
	{
		mbDraging = false;
	}

	Control::Control(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,rc)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
		SetHoverBackColor(hoverBackColor);
		SetHoverForceColor(hoverForceColor);
		mBDownInternal = false;
		mbDraging = false;
	}

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

	UINT ButtonInterface::HitTest(Listener * pListener,POINT pt)
	{
		return HTCLIENT;
	}

	ButtonInterface::ButtonInterface() :ButtonInterface(L"",nullRect)
	{

	}

	ButtonInterface::ButtonInterface(wstring text, RECT rc):Control(text,rc),mBoardColor(mBackColor), mDrawBoard(false)
	{
	}

	ButtonInterface::ButtonInterface(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,
		rc,forceColor,backColor,hoverForceColor,hoverBackColor), mBoardColor(mBackColor), mDrawBoard(false)
	{
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
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor),&mpTextpLayout,true);
			IS_ERROR_EXIT(!ret, "Draw TitleBar failed!");
		}
		else
		{
			auto ret = DrawManager.DrawRectWithTextW(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor),&mpTextpLayout, true);
			IS_ERROR_EXIT(!ret, "Draw  honvered TitleBar failed!");
		}
		mIconSprite[0].Render();
		mIconSprite[1].Render();
	}


	UINT TitleBar::HitTest(Listener * pListener, POINT pt)
	{
		return HTCAPTION;
	}

	void TitleBar::Sizing(RECT newRect)
	{
		auto titleBarHeight = height();
		AdjustRect(MyRect(0, 0, RECTWIDTH(newRect)-3*titleBarHeight, titleBarHeight));
	}

	TitleBar::TitleBar():Control(L"", nullRect)
	{
		SetID(CONTROL_TYPE_TITLEBAR);
		//test sprite
		mIconSprite[0].LoadEx(L"effect/hit/*.png");
		mIconSprite[0].ChangeRect(MyRect(0,300,256,556));
		mIconSprite[0].SetSpeed(20);

		mIconSprite[1].LoadEx(L"effect/hit2/*.png");
		mIconSprite[1].ChangeRect(MyRect(556, 300, 812, 556));
		mIconSprite[1].SetSpeed(20);
	}

	TitleBar::TitleBar(wstring text,RECT rc):Control(text,rc)
	{
		SetID(CONTROL_TYPE_TITLEBAR);
	}

	TitleBar::TitleBar(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
	}

	TitleBar::~TitleBar()
	{

	}

	Button::Button():ButtonInterface()
	{
		SetID(CONTROL_TYPE_DEFAULT_BUTTON);
	}

	CloseButton::CloseButton():ButtonInterface()
	{
		SetID(CONTROL_TYPE_CLOSE_BUTTON);
	}

	CloseButton::CloseButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):ButtonInterface(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		CloseButton();
	}

	void CloseButton::Draw(Listener * pListener)
	{
		RECT drawRect = Conver::ClipRectBoard(mRect, 10, 10);

		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawLine(LeftTopPoint(drawRect),RightBottomPoint(drawRect), COLOREX(mForceColor));
			DrawManager.DrawLine(LeftBottomPoint(drawRect),RightTopPoint(drawRect), COLOREX(mForceColor));
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

	MiniButton::MiniButton():ButtonInterface()
	{
		SetID(CONTROL_TYPE_MINI_BUTTON);
	}

	MiniButton::MiniButton(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):ButtonInterface(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		MiniButton();
	}

	void MiniButton::Draw(Listener * pListener)
	{
		RECT drawRect = ClipRectBoard(mRect, 10, 24);
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
		RECT drawRect = ClipRectBoard(mRect, 12, 12);
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
		RECT drawRect = ClipRectBoard(mRect, 13, 13);
		auto centerPt = CenterPoint(drawRect);
		RECT rightTopRect = { centerPt.x , centerPt.y - 9, centerPt.x + 9,centerPt.y };
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

	void ImageButton::Draw(Listener * pListener)
	{
		IS_RETURN_ERROR(!pImage,,"ImageButton Í¼ÏñÎª¿Õ");
		if (pHonverImage == nullptr) pHonverImage = pImage;
		if (ZeroRect(mImgRec)) mImgRec = mRect;
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect,mBackColor,true);
			DrawManager.DrawPicture(pImage, mImgRec);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, mHonverBackColor, true);
			DrawManager.DrawPicture(pHonverImage, mImgRec);
		}
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



	////////////////////////////////////////////////////////////////////////// EditBox
	EditBox::EditBox(RECT rc,string defaultText)
	{
		mForceColor = mHoverForceColor = RGB(255,255,255);
		mBackColor = mBackColor = RGB(116,116,119);
	}

	EditBox::EditBox():Control()
	{
		mOwnCaret = true;
	}

	EditBox::EditBox(wstring defaultText, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor) :Control(defaultText,
		rc, forceColor, backColor, hoverForceColor, hoverBackColor) {
		mOwnCaret = true; //must have caret
	}

	void EditBox::ChangeFrontSize(float newSize)
	{
		mFrontSize = newSize;
	}

	void EditBox::ChangeFrontName(wstring newFrontName)
	{
		mFrontName = newFrontName;
	}

	void EditBox::Draw(Listener * pListener)
	{
		DrawManager.DrawRectangle(mRect,COLOREX(mBackColor),true);
		DrawManager.DrawRectangle(mRect,mBoardColor, false); //draw board
		DrawManager.DrawTextW(mText, mRect, mForceColor, STCAST(float, RECTHEIGHT(mRect) - ALIGN_UPDPWNDISTANCE), &mpTextpLayout,ALIGN_TEXT_LEFT,ALIGN_PARAGRAPH_CENTER);
	}
	
	UINT EditBox::HitTest(Listener * pListener, POINT pt)
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


////////////////////////////////////////////////////////////////////////// LableBox
	LabelBox::LabelBox():Control(L"")
	{
	}

	void LabelBox::Draw(Listener * pListener)
	{
		Control::Draw(pListener);
		DrawManager.DrawTextW(mText, mRect, mForceColor, STCAST(float,RECTHEIGHT(mRect)-ALIGN_UPDPWNDISTANCE),&mpTextpLayout);
	}

	UINT LabelBox::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}



}
