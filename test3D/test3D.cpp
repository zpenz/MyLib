#include "stdafx.h"
#include "../LIB3D/3D.h"
#include "../Mylib/Macros.h"

#include "../MyLib/2Draw.h"

using test = bool(*)(void);

#include "../Http/Http.h"
#pragma comment(lib,"../debug/Http.lib")
#pragma comment(lib,"../debug/lib.lib")

int main()
{
	std::string strRet;
	//HttpManager.Post("https://store.steampowered.com/login/rendercaptcha/","gid=3007723630221818606", strRet);
	
	IS_RETURN_ERROR(!DrawManager.InitManager(),0 , "≥ı ºªØD2D¥ÌŒÛ!");
	IS_RETURN_ERROR(!DrawManager.SetRenderTarget(GetDesktopWindow()), 0, "…Ë÷√RenderTarget ß∞‹!");
	DrawManager.UseTempRenderTarget();
	
	HttpManager.DownLoadImage("https://store.steampowered.com/login/rendercaptcha/gid=3007723630221818606.png");

	printf_s("%s",strRet.c_str()); 
    return 0;
}

