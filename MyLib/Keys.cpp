# include "stdafx.h"
# include "lib.h"


extern void Error_Box(const char * _error);
/*
============
void SetPress
���ð�������
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
���ð����ͷ�
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
����Ƿ����˸ð���
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
		Error_Box("����һ������̽�����!");
		exit(0);
		return -1;
	}
	
}

/*
===================
���캯������������
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