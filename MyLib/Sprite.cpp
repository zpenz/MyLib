#include "stdafx.h"
#include "Sprite.h"
#include <algorithm>
#include <io.h>
#include <list>
#include <memory>

namespace Sprite {

	bool Sprite::IsAlreayRun() const
	{
		return mAlreadyRun;
	}

	void Sprite::Stop()
	{
		mPause = true;
	}

	void Sprite::Reseume()
	{
		mPause = false;
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
		if (mPause) return;
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

	bool Sprite::LoadEx(wstring dirPath)
	{
		wstring dir = Conver::GetDirName(dirPath);

		struct _wfinddata_t fileInfo;
		auto handle = _wfindfirst(dirPath.c_str(), &fileInfo);

		shared_ptr<void> pEixt(NULL, [&handle](void *) {
			if (handle) _findclose(handle);
		});

		IS_RETURN_ERROR(handle==-1,false,"遍历文件错误! 可能文件夹为空");
		list<wstring> fileList;
		fileList.emplace_back(fileInfo.name);
		while (!_wfindnext(handle, &fileInfo)) // 成功返回0 否则-1
		{
			fileList.emplace_back(fileInfo.name);
		}

		//排序
		fileList.sort();
		for (auto element : fileList)
		{
			LoadAFrame(dir + element);
		}
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

	Sprite::Sprite():mCurrentFrame(0),mTotalFrames(0),mIsLoop(true),mPlaySpeed(1.0),mIsDead(false),mAlreadyRun(false),mPause(false)
	{
	}


	Sprite::~Sprite()
	{
		for_each(mFrameList.begin(), mFrameList.end(), [](IPIC * pBtimap) {
			SAFE_RELEASE(pBtimap);
		});
	}

}
