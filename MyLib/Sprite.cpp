#include "stdafx.h"
#include "Sprite.h"
#include <algorithm>

namespace Sprite {

	bool Sprite::IsAlreayRen() const
	{
		return mAlreadyRun;
	}

	IPIC * Sprite::getCurrentFrame()
	{
		return mFrameList[mCurrentFrame];
	}

	unsigned int Sprite::Inc()
	{
		if (mCurrentFrame < mTotalFrames-1) mCurrentFrame++;
		else
		{
			if (!mIsLoop) mIsDead = true;
			mCurrentFrame = 0; 
		}
		return mCurrentFrame;
	}

	float Sprite::getSpeed() const
	{
		return mPlaySpeed;
	}

	void Sprite::SetLoop(bool isLoop)
	{
		mIsLoop = isLoop;
	}

	bool Sprite::Loop()
	{
		return mIsLoop;
	}

	bool Sprite::Dead()
	{
		return mIsDead;
	}

	void Sprite::SetState(bool state)
	{
		mIsDead = state;
	}

	void Sprite::SetSpeed(float newSpeed)
	{
		mPlaySpeed = newSpeed;
	}

	bool Sprite::LoadAFrame(wstring filename)
	{
		auto newFrame = DrawManager.CreateBitmap(const_cast<wchar_t *>(filename.c_str()));
		if (!newFrame) return false;
		mFrameList.push_back(newFrame);
		mTotalFrames++;
		return true;
	}

	bool Sprite::Load(unsigned int size, ...)
	{
		va_list va;
		va_start(va, size);
		
		for (auto it = 0; it < size; it++)
		{
			wchar_t * filename = va_arg(va,wchar_t*);
			LoadAFrame(filename);
		}

		va_end(va);
		return false;
	}

	bool Sprite::Render()
	{
		DrawManager.DrawPicture(getCurrentFrame(), mRect);
		//Sleep(static_cast<int>(1.0 / this->getSpeed() * 1000));
		this->Inc();

		return true;
	}

	void Sprite::ChangeRect(RECT rc)
	{
		mRect = rc;
	}

	RECT & Sprite::Rect()
	{
		return mRect;
	}

	Sprite::Sprite():mCurrentFrame(0),mTotalFrames(0),mIsLoop(true),mPlaySpeed(1.0),mIsDead(false),mAlreadyRun(false)
	{
	}


	Sprite::~Sprite()
	{
		for_each(mFrameList.begin(), mFrameList.end(), [](IPIC * pBtimap) {
			SAFE_RELEASE(pBtimap);
		});
	}

}
