#pragma once
#include "Macros.h"

namespace MY_LIB
{
	using namespace std;

	class Semaphore
	{
	private:
		string mSemaphoreName;
		long mValue;
		long mCout;
		HANDLE mHandle;

	public: 
		bool Add(UINT increase = 1);
		bool Reduce(UINT reduce = 1);
		static HANDLE GetSemphore(string SemaphoreName);
		Semaphore(string SemaphoreName,long initValue=0,long maxCount=0);
		Semaphore(string SemaphoreName);
		~Semaphore();
	};

}


