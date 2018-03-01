#include "stdafx.h"
#include "Sprite.h"
#include <algorithm>

namespace Sprite {

	bool Sprite::IsAlreayRun() const
	{
		return mAlreadyRun;
	}

	IPIC * Sprite::getCurrentFrame() const
	{
		return mFrameList[mCurrentFrame];
	}

	bool Sprite::Inc()
	{
		auto changeFrame = [](LPVOID  lpParam)->DWORD
		{
			auto pSprite = (Sprite *)lpParam;
			while (!pSprite->Dead())
			{
				Sleep(static_cast<int>(1.0 / pSprite->getSpeed() * 1000));
				pSprite->IncreaseFrame(1);
			}

			return 0;
		};

		if (IsAlreayRun()) return false;

		DWORD threadId = 0;
		auto handle = CreateThread(NULL, 0, changeFrame, this, 0, &threadId);
		if (!handle) return false;
		CloseHandle(handle);

		mAlreadyRun = true;
		return true;
	}

	void Sprite::IncreaseFrame(unsigned int skipFrame)
	{
		if(skipFrame+mCurrentFrame<mTotalFrames)
		{
			mCurrentFrame += skipFrame;
		}
		else
		{
			if (!mIsLoop) SetState(true);
			else mCurrentFrame = 0;
		}
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
		
		for (unsigned int it = 0; it < size; it++)
		{
			wchar_t * filename = va_arg(va,wchar_t*);
			LoadAFrame(filename);
		}

		va_end(va);
		return false;
	}

	bool Sprite::Render()
	{
		if (mTotalFrames == 0) return false;
		DrawManager.DrawPicture(getCurrentFrame(), mRect);

		Inc();  

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
