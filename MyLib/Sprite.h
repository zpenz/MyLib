#pragma once
#include <vector>
#include "2Draw.h"

namespace Sprite
{
	using namespace std;

	class Sprite
	{
	protected:

		unsigned int mCurrentFrame;

		unsigned int mTotalFrames;

		vector<IPIC *> mFrameList;

		float mPlaySpeed;

		bool mIsLoop;

		bool mIsDead;

		bool mPause; 

		RECT mRect;

		bool mAlreadyRun;

		bool Inc();

	public:
		bool IsAlreayRun() const;

		IPIC * getCurrentFrame() const;

		void IncreaseFrame(unsigned int skipFrame);

		float getSpeed() const;

		void SetLoop(bool isLoop);

		bool Loop();

		bool Dead();

		void Stop();

		void Reseume();

		void SetState(bool state);

		void SetSpeed(float newSpeed);

		bool LoadAFrame(wstring filename);

		bool Load(unsigned int size,...);

		bool LoadEx(wstring dirPath); //load dir

		bool Render(); 

		void ChangeRect(RECT rc);

		RECT & Rect();

		Sprite();

		~Sprite();
	};
}



