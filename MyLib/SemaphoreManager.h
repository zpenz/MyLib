#pragma once
#include <map>
#include <string>
#include "Macros.h"

namespace MY_LIB
{
	using namespace std;
	//waring: 同名对象
	class Semaphore
	{
	private:
		string mSemaphoreName;
		long mValue;
		long mCout;
		HANDLE mHandle;

	public: // if private will cause a problem with map<Semaphore,int >
		//错误	C2280	“void *std::pair<const _Kty, _Ty>::__delDtor(unsigned int)”: 尝试引用已删除的函数
		// see down
		bool Add(UINT increase = 1);
		bool Reduce(UINT reduce = 1);
		HANDLE GetSemphore(string SemaphoreName);
		Semaphore(string SemaphoreName,long initValue=0,long maxCount=0);
		Semaphore(string SemaphoreName);
		~Semaphore();
	};

}


