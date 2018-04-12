#include "Control.h"

namespace LIB_CONTROL
{
	using namespace Conver;

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
				if (pControl->mClickFunc) pControl->mClickFunc(); //调用点击事件
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
		CaretManager.isTrial = true;
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
		if (!mpTextpLayout) return;
		if (!mFocusCaret) return;

		auto index = CaretManager.getIndex();
		auto CaretPosition = CaretManager.pos();
		Point tempPoint; 

		if (mText.empty()) return;
		if (index < 0 || index > mText.size() - 1) return;
		if (cUnicode == '\b') 
		{
			mText.erase(index,1); 	
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
			CaretManager.DecIndex();

			CaretManager.isTrial = true;
		}
		else if (cUnicode == VK_LEFT)
		{   
			if (index == 0) {
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
				CaretManager.isTrial = false;
				return;
			}
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, false);
			CaretManager.DecIndex();
			
			//坐标没变 在调整一次
			tempPoint = CaretManager.pos();
			if (tempPoint.x == CaretPosition.x && tempPoint.y == CaretPosition.y)
			{
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex(), false);
				CaretManager.DecIndex();
			}
				
			CaretManager.isTrial = false;
		}
		else if (cUnicode == VK_RIGHT)
		{
			
			if (index == mText.size()-1) {
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, true);
				CaretManager.isTrial = true;
				return;
			}
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, index, true);
			CaretManager.IncIndex();

			//坐标没变 在调整一次
			tempPoint = CaretManager.pos();
			if (tempPoint.x == CaretPosition.x && tempPoint.y == CaretPosition.y)
			{
				CaretManager.SetCaretPosEx(mRect, mpTextpLayout, CaretManager.getIndex(), true);
				CaretManager.IncIndex();
			}
			CaretManager.isTrial = true;
		}
		else
		{
			auto posInsert = index;
			if (CaretManager.isTrial) posInsert = index + 1;
			//添加字符
			if (mText.empty()) mText = wstring(&cUnicode);
			else
			{
				mText = mText.insert(posInsert, wstring(&cUnicode));
			}
			Draw(pListener);
			CaretManager.SetCaretPosEx(mRect, mpTextpLayout, posInsert, true);
			CaretManager.IncIndex();
			CaretManager.isTrial = true;
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

	COLORREF & Control::getBoardColor()
	{
		return mBoardColor;
	}

	COLORREF & Control::ForceColor() 
	{
		return mForceColor;
	}

	void Control::SetForceColor(COLORREF color)
	{
		mForceColor = color;
	}

	COLORREF & Control::BackColor() 
	{
		return mBackColor; 
	}

	void Control::SetBackColor(COLORREF color)
	{
		mBackColor = color;
	}

	COLORREF & Control::HoverBackColor() 
	{
		return mHonverBackColor;
	}

	void Control::SetHoverBackColor(COLORREF color)
	{
		mHonverBackColor = color;
	}

	COLORREF & Control::HoverForceColor() 
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
	}

	Control::Control():Control(L"",nullRect)
	{
	}

	Control::Control(wstring text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,rc)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
		SetHoverBackColor(hoverBackColor);
		SetHoverForceColor(hoverForceColor);
		mBDownInternal = false;
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

	TitleBar::TitleBar()
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
		if (!pImage) if(!mText.empty()) LoadFromFile(mText); //如果文字非空，那么就加载文字路径下的图
		IS_RETURN_ERROR(!pImage,,"ImageButton 图像为空");
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

	ImageButton::ImageButton()
	{
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
	LabelBoxInterface::LabelBoxInterface()
	{
	}

	void LabelBoxInterface::Draw(Listener * pListener)
	{
		Control::Draw(pListener);
		DrawManager.DrawTextW(mText, mRect, mForceColor, STCAST(float,RECTHEIGHT(mRect)-ALIGN_UPDPWNDISTANCE),&mpTextpLayout);
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

	LabelBox::LabelBox(){}

}
