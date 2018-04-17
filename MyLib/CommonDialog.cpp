#include "stdafx.h"
#include "CommonDialog.h"
#include <Shobjidl.h>

namespace CommonItem
{
	COMDLG_FILTERSPEC filters[] = {
		{ L"布局文件", L"*.layout" }
	};

	wstring CommonDialog::ShowOpenFileDialog(bool clearRe)
	{
		IFileDialog *pfd = NULL;
		auto ret = CoInitialize(NULL);
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		IS_RETURN(FAILED(hr), L"");

		if (clearRe) pfd->ClearClientData();
		IShellItem * item = NULL;
		hr = SHCreateItemFromParsingName(L"C:\\", NULL, IID_PPV_ARGS(&item));
		pfd->SetDefaultFolder(item);
		pfd->SetFileTypes(1,filters);
		pfd->Show(NULL);
		pfd->GetResult(&item);
		LPWSTR strOpenPath = NULL;
		hr = item->GetDisplayName(SIGDN_FILESYSPATH, &strOpenPath);
		item->Release();
		pfd->Release();

		CoUninitialize();
		return strOpenPath;
	}

	wstring CommonDialog::ShowSaveFileDialog(bool clearRe)
	{
		IFileSaveDialog *pfd = NULL;
		auto ret = CoInitialize(NULL);
		HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		IS_RETURN(FAILED(hr),L"");

		if (clearRe) pfd->ClearClientData();
		IShellItem * item = NULL;
		hr = SHCreateItemFromParsingName(L"C:\\", NULL, IID_PPV_ARGS(&item));
		pfd->SetDefaultFolder(item);
		pfd->SetFileTypes(1, filters);
		pfd->Show(NULL);
		pfd->GetResult(&item);
		LPWSTR strSavePath = NULL;
		hr = item->GetDisplayName(SIGDN_FILESYSPATH, &strSavePath);
		item->Release();
		pfd->Release();

		CoUninitialize();
		return strSavePath;
	}

	CommonDialog::~CommonDialog()
	{
	}

}
