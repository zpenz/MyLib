#pragma once
#include <vector>
#include "2Draw.h"

namespace Sprite
{
	using namespace std;

	class Sprite
	{
		unsigned int mCurrentFrame;

		unsigned int mTotalFrames;

		vector<IPIC *> mFrameList;

		float mPlaySpeed;

		bool mIsLoop;

		bool mIsDead;

		RECT mRect;

		bool mAlreadyRun;

	public:
		bool IsAlreayRun() const;

		IPIC * getCurrentFrame() const;

		bool Inc();  

		void IncreaseFrame(unsigned int skipFrame);

		float getSpeed() const;

		void SetLoop(bool isLoop);

		bool Loop();

		bool Dead();

		void SetState(bool state);

		void SetSpeed(float newSpeed);

		bool LoadAFrame(wstring filename);

		bool Load(unsigned int size,...);

		bool Render(); //only one time run...

		void ChangeRect(RECT rc);

		RECT & Rect();

		Sprite();

		~Sprite();
	};
}



