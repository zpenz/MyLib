#include "stdafx.h"
#include "SemaphoreManager.h"

namespace MY_LIB
{
	bool Semaphore::Add(UINT increase)
	{
		ReleaseSemaphore(mHandle, increase, NULL);
		mValue++;
		return false;
	}
	bool Semaphore::Reduce(UINT reduce)
	{
		WaitForSingleObject(mHandle,INFINITE);
		mValue--;
		return false;
	}

	HANDLE Semaphore::GetSemphore(string SemaphoreName)
	{
		auto ret = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, SemaphoreName.c_str());
		if (!ret) LOG_WARNING("OpenSemphore: 无法打开指定的SemaphoreObject");
		return ret;
	}

	Semaphore::Semaphore(string SemaphoreName, long initValue, long maxCount):mSemaphoreName(SemaphoreName),
		mValue(initValue),mCout(maxCount)
	{
		mHandle=CreateSemaphoreA(NULL,initValue,maxCount,SemaphoreName.c_str());
		IS_RETURN_ERROR(!mHandle,, "create semphore failed!");
	}

	Semaphore::Semaphore(string SemaphoreName) :Semaphore(SemaphoreName, 0, 1)
	{

	}

	Semaphore::~Semaphore()
	{
		CloseHandle(mHandle);
	}

}
