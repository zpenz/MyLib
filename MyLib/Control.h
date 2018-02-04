#pragma once
#include <list>
#include <windows.h>
#include "Macros.h"

namespace LIB_CONTROL
{
	using namespace std;

	class Listener;

	class Control
	{
	public:
		virtual void Draw(Listener * pListener) = 0;
		virtual void Hover(Listener * pListener) = 0;
		virtual void LButtonDown(Listener * pListener) = 0;
		virtual void LButtonUp(Listener * pListener) = 0;
		virtual ~Control();
	};

	class Listener
	{
		list<Control *> mpControl;
	public:
		bool attach(Control * pControl);
		bool detach(Control * pControl);
	};

}



