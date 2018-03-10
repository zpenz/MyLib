#include "Control.h"


namespace LIB_CONTROL
{

	bool DragAdapter::IsDraging()
	{
		return mBDraging;
	}

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
		
	UINT Listener::LButtonDown(POINT pt)
	{
		Control * pTempControl = nullptr;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
				if(Conver::PointInRect(pt.x, pt.y, pControl->getRect()))
				  if(pControl->LButtonDown(this)!=SHOULD_DO_NOTHING) pTempControl = pControl;
		});
		if(pTempControl != nullptr) return pTempControl->LButtonDown(this);
		return SHOULD_DO_NOTHING;
	}

	UINT Listener::LButtonUp(POINT pt)
	{
		UINT ret = SHOULD_DO_NOTHING;
		for_each(mpControl.begin(), mpControl.end(), [&](Control * pControl) {
			if (Conver::PointInRect(pt.x, pt.y, pControl->getRect()))
				ret = pControl->LButtonUp(this);
				if (ret != SHOULD_DO_NOTHING) return ret; 
				return ret;
		});
		return ret;
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

	void Control::Sizing(RECT newRect)
	{

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

	LONG Control::width() const
	{
		return mRect.right - mRect.left;
	}

	LONG Control::height() const
	{
		return mRect.bottom - mRect.top;
	}

	Control::Control():Control("")
	{

	}
	
	Control::Control(string text, RECT rc):mRect(rc),mText(text),mVisible(true),mBackColor(RGB(45,45,48)),
		mForceColor(RGB(110,110,112)), mHonverBackColor(RGB(63, 63, 65)),mHoverForceColor(RGB(110, 110, 112)),
		mAlignType(ALIGN_CENTER_V|ALIGN_CENTER_H),mCanStretch(false)
	{

	}

	Control::Control(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,rc)
	{
		SetForceColor(forceColor);
		SetBackColor(backColor);
		SetHoverBackColor(hoverBackColor);
		SetHoverForceColor(hoverForceColor);
	}

	void Button::SetButtonDownInternal(bool isDownInternal)
	{
		BDownInternal = isDownInternal;
	}

	void Button::SetBoardColor(COLORREF cBoardColor)
	{
		mBoardColor = cBoardColor;
	}

	void Button::NeedDrawBoard(bool bDrawBoard)
	{
		mBoardColor = bDrawBoard;
	}

	void Button::Draw(Listener * pListener)
	{

	}

	void Button::Hover(Listener * pListener,POINT pt)
	{

	}

	UINT Button::LButtonDown(Listener * pListener)
	{
		SetButtonDownInternal(false);
		if (IsMouseInteral()) SetButtonDownInternal(true);
		return SHOULD_DO_NOTHING;
	}

	UINT Button::LButtonUp(Listener * pListener)
	{
		return SHOULD_DO_NOTHING;
	}

	UINT Button::HitTest(Listener * pListener,POINT pt)
	{
		return HTCLIENT;
	}

	Button::Button():mBoardColor(mBackColor),mDrawBoard(false),BDownInternal(false)// ±ß¿òÊ¹ÓÃ±³¾°ÑÕÉ«
	{
		SetID(CONTROL_TYPE_BUTTON);
		SetHoverForceColor(mForceColor);
	}

	Button::Button(string text, RECT rc):Control(text,rc),mBoardColor(mBackColor), mDrawBoard(false), BDownInternal(false)
	{

	}

	Button::Button(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text,
		rc,forceColor,backColor,hoverForceColor,hoverBackColor), mBoardColor(mBackColor), mDrawBoard(false), BDownInternal(false)
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
		mIconSprite.ChangeRect(Conver::MyRect(100, 100, 800, 800));
		if (!mMouseInternal)
		{
			auto ret = DrawManager.DrawRectWithText(mRect, mText, COLOREX(mBackColor), COLOREX(mForceColor), ALIGN_DEFAULT, true);
			IS_ERROR_EXIT(!ret, "Draw TitleBar failed!");
		}
		else
		{
			auto ret = DrawManager.DrawRectWithText(mRect, mText, COLOREX(mHonverBackColor), COLOREX(mHoverForceColor), ALIGN_DEFAULT, true);
			IS_ERROR_EXIT(!ret, "Draw  honvered TitleBar failed!");
		}
		mIconSprite.Render();
	}

	void TitleBar::Hover(Listener * pListener,POINT pt)
	{
	}

	UINT TitleBar::LButtonDown(Listener * pListener)
	{
		return SHOULD_DO_NOTHING;
	}

	UINT TitleBar::LButtonUp(Listener * pListener)
	{
		return SHOULD_DO_NOTHING;
	}

	UINT TitleBar::HitTest(Listener * pListener, POINT pt)
	{
		return HTCAPTION;
	}

	void TitleBar::Sizing(RECT newRect)
	{
		auto titleBarHeight = height();
		AdjustRect(Conver::MyRect(0, 0, RECTWIDTH(newRect)-3*titleBarHeight, titleBarHeight));
	}

	TitleBar::TitleBar(string text,RECT rc):Control(text,rc)
	{
		SetID(CONTROL_TYPE_TITLEBAR);
		//test
		mIconSprite.LoadEx(L"effect/hit/*.png");
		mIconSprite.SetSpeed(30);
	}

	TitleBar::TitleBar(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Control(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {}

	TitleBar::~TitleBar()
	{

	}

	CloseButton::CloseButton()
	{
		SetID(CONTROL_TYPE_CLOSE_BUTTON);
	}

	CloseButton::CloseButton(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Button(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		CloseButton();
	}

	void CloseButton::Draw(Listener * pListener)
	{
		RECT drawRect = Conver::ClipRectBoard(mRect, 10, 10);

		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawLine(Conver::LeftTopPoint(drawRect), Conver::RightBottomPoint(drawRect), COLOREX(mForceColor));
			DrawManager.DrawLine(Conver::LeftBottomPoint(drawRect), Conver::RightTopPoint(drawRect), COLOREX(mForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawLine(Conver::LeftTopPoint(drawRect), Conver::RightBottomPoint(drawRect), COLOREX(mHoverForceColor));
			DrawManager.DrawLine(Conver::LeftBottomPoint(drawRect), Conver::RightTopPoint(drawRect), COLOREX(mHoverForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}

	void CloseButton::Hover(Listener * pListener, POINT pt)
	{

	}

	UINT CloseButton::LButtonDown(Listener * pListener)
	{
		SetButtonDownInternal(false);
		if (IsMouseInteral()) SetButtonDownInternal(true);
		return SHOULD_DO_NOTHING;
	}

	UINT CloseButton::LButtonUp(Listener * pListener)
	{
		if (BDownInternal) return SHOULD_CLOSE_WINDOW;
		return SHOULD_DO_NOTHING;
	}

	UINT CloseButton::HitTest(Listener * pListener, POINT pt)
	{
		return HTCLIENT;
	}

	void CloseButton::Sizing(RECT newRect)
	{
		auto btnHeight = height();
		AdjustRect(Conver::MyRect(RECTWIDTH(newRect)-btnHeight,0, RECTWIDTH(newRect), btnHeight));
	}

	MiniButton::MiniButton()
	{
		SetID(CONTROL_TYPE_MINI_BUTTON);
	}

	MiniButton::MiniButton(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Button(text, rc, forceColor, backColor,
		hoverForceColor, hoverBackColor) {
		MiniButton();
	}

	void MiniButton::Draw(Listener * pListener)
	{
		RECT drawRect = Conver::ClipRectBoard(mRect, 10, 24);
		if (!mMouseInternal)
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mBackColor), true);
			DrawManager.DrawLine(Conver::LeftTopPoint(drawRect), Conver::RightTopPoint(drawRect), COLOREX(mForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
		else
		{
			DrawManager.DrawRectangle(mRect, COLOREX(mHonverBackColor), true);
			DrawManager.DrawLine(Conver::LeftTopPoint(drawRect), Conver::RightTopPoint(drawRect), COLOREX(mHoverForceColor));
			if (mDrawBoard) DrawManager.DrawRectangle(mRect, COLOREX(mBoardColor), false);
		}
	}


	UINT MiniButton::LButtonUp(Listener * pListener)
	{
		if (BDownInternal) return SHOULD_MINI_WINDOW;
		return SHOULD_DO_NOTHING;
	}

	void MiniButton::Sizing(RECT newRect)
	{
		auto btnHeight = height();
		AdjustRect(Conver::MyRect(RECTWIDTH(newRect) - 3*btnHeight, 0, RECTWIDTH(newRect)-2*btnHeight, btnHeight));
	}

	void MaxButton::DrawMaxImg()
	{
		RECT drawRect = Conver::ClipRectBoard(mRect, 12, 12);
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
		RECT drawRect = Conver::ClipRectBoard(mRect, 13, 13);
		auto centerPt = Conver::CenterPoint(drawRect);
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

	MaxButton::MaxButton():isMax(false)
	{
		SetID(CONTROL_TYPE_MAXI_BUTTON);
	}

	MaxButton::MaxButton(string text, RECT rc, COLORREF forceColor, COLORREF backColor, COLORREF hoverForceColor, COLORREF hoverBackColor):Button(text, rc, forceColor, backColor,
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

	UINT MaxButton::LButtonUp(Listener * pListener)
	{
		if (BDownInternal) 
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
		AdjustRect(Conver::MyRect(RECTWIDTH(newRect) - 2*btnHeight, 0, RECTWIDTH(newRect) - btnHeight, btnHeight));
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
		for_each(mControl.begin(), mControl.end(), [&pListener](Control * ItControl){
			ItControl->Draw(pListener);
		});
	}

	UINT ComposeControl::HitTest(Listener * pListener, POINT pt)
	{
		for_each(mControl.begin(), mControl.end(), [&pListener,pt](Control * ItControl) {
			ItControl->HitTest(pListener,pt);
		});
		return 0;
	}

	void ComposeControl::Hover(Listener * pListener, POINT pt)
	{
		for_each(mControl.begin(), mControl.end(), [&pListener, pt](Control * ItControl) {
			ItControl->Hover(pListener, pt);
		});
	}

	UINT ComposeControl::LButtonDown(Listener * pListener)
	{
		for_each(mControl.begin(), mControl.end(), [&pListener](Control * ItControl) {
			ItControl->LButtonDown(pListener);
		});		
		return 0;
	}

	UINT ComposeControl::LButtonUp(Listener * pListener)
	{
		for_each(mControl.begin(), mControl.end(), [&pListener](Control * ItControl) {
			ItControl->LButtonUp(pListener);
		});
		return 0;
	}

	void ImageButton::Draw(Listener * pListener)
	{
		IS_RETURN_ERROR(!pImage,,"ImageButton Í¼ÏñÎª¿Õ");
		if (pHonverImage == nullptr) pHonverImage = pImage;
		if (Conver::ZeroRect(mImgRec)) mImgRec = mRect;
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
}
