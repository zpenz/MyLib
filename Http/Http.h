#pragma once

#ifdef PSONGAPI
#define PSONGAPI _declspec(dllimport)
#else 
#define PSONGAPI _declspec(dllexport)
#endif 

#include "../MyLib/Macros.h"


extern "C"
{
	class PSONGAPI HttpClass
	{
	public:

		bool Post(std::string strHostUrl,std::string strParams,std::string & strRetData);

		bool DownLoadImage(std::string strImageUrl,std::string strSavePath="image.png");

		SINGLE_INSTANCE(HttpClass)
	};

#define HttpManager HttpClass::getInstance()
}