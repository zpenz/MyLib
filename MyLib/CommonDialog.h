#pragma once
#include "Macros.h"

namespace CommonItem
{

	using namespace std;

	class CommonDialog
	{
		SINGLE_INSTANCE(CommonDialog)
	public:

		wstring ShowOpenFileDialog(bool clearRe = false);

		wstring ShowSaveFileDialog(bool clearRe = false);

		~CommonDialog();
	};

#define DlgManager CommonDialog::getInstance() 
}

