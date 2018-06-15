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

	auto hwndNode = FindWindowA(NULL, "�½��ı��ĵ� (2) - ���±�");
	
	IS_RETURN_ERROR(!DrawManager.InitManager(),0 , "��ʼ��D2D����!");
	IS_RETURN_ERROR(!DrawManager.SetRenderTarget(hwndNode), 0, "����RenderTargetʧ��!");
	//DrawManager.UseTempRenderTarget();
	
	HttpManager.DownLoadImage("https://store.steampowered.com/login/rendercaptcha/gid=3007723630221818606.png");

	DrawManager.DrawPicture(L"image.png", Conver::MyRect(0,0,100,50));
	printf_s("%s",strRet.c_str()); 
    return 0;
}

