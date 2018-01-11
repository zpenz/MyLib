# include "stdafx.h"
# include "lib.h"


extern void Error_Box(const char * _error);
/*
============
void SetPress
设置按键按下
============
*/
void Keys::SetPress(unsigned int key)
{
	if(key<MAX_KEYS && key>=0)
	{
		keys[key]=true;
	}
	else
	{
		return;
	}
}
/*
===========
void SetRelease
设置按键释放
===========
*/
void Keys::SetRelease(unsigned int key)
{
	if(key<MAX_KEYS && key>=0)
	{
		keys[key]=false;
	}
	return ;
}

/*
==============
bool IsPress
检查是否按下了该按键
==============
*/
bool Keys::IsPress(unsigned int key)
{
	if(key<MAX_KEYS && key>=0)
	{
		return keys[key];
	}
	else
	{
		Error_Box("发生一个按键探查错误!");
		exit(0);
		return -1;
	}
	
}

/*
===================
构造函数和析构函数
===================
*/
Keys::Keys()
{
	for(int i = 0;i<256;i++)
	{
		keys[i]=false;
	}
	this->LButtonDown = false;
	this->LButtonUp   = false;
	this->RButtonDown = false;
	this->RButtonDUp  = false;
}

Keys::~Keys()
{
	for(int i = 0;i<256;i++)
	{
		keys[i]=0;
	}
}