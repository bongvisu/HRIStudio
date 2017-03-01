#include "hsuiPrecompiled.h"
#include "hsuiPropGridFileProperty.h"
#include "hsuiPropGridCtrl.h"
#include "hsuiVisualManager.h"

#undef min
#undef max

namespace hsui {

IMPLEMENT_DYNAMIC(PropGridFileProperty, PropGridProperty)

PropGridFileProperty::PropGridFileProperty(
	const CString& strName,
	const CString& strFolderName,
	DWORD_PTR dwData,
	LPCTSTR lpszDescr)
	:
	PropGridProperty(strName, COleVariant((LPCTSTR)strFolderName), lpszDescr, dwData),
	mIsFolder(TRUE)
{
	mFlags = PROP_HAS_BUTTON;
}

PropGridFileProperty::PropGridFileProperty(
	const CString& strName,
	BOOL bOpenFileDialog,
	const CString& strFileName,
	LPCTSTR lpszDefExt,
	DWORD dwFileFlags,
	LPCTSTR lpszFilter,
	LPCTSTR lpszDescr,
	DWORD_PTR dwData)
	:
	PropGridProperty(strName, COleVariant((LPCTSTR)strFileName), lpszDescr, dwData),
	mOpenFileDialog(bOpenFileDialog), mFileOpenFlags(dwFileFlags)
{
	mFlags = PROP_HAS_BUTTON;
	mDefaultExtension = lpszDefExt == nullptr ? _T("") : lpszDefExt;
	mFilterString = lpszFilter == nullptr ? _T("") : lpszFilter;

	mIsFolder = FALSE;
}

PropGridFileProperty::~PropGridFileProperty()
{
}

void PropGridFileProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);
	ASSERT_VALID(mWndInPlace);
	ASSERT(::IsWindow(mWndInPlace->GetSafeHwnd()));

	mButtonIsDown = TRUE;
	Redraw();

	CString strPath = mValue.bstrVal;
	BOOL bUpdate = FALSE;

	if (mIsFolder)
	{
		if (afxShellManager == nullptr)
		{
			CWinAppEx* app = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());
			if (app != nullptr)
			{
				app->InitShellManager();
			}
		}

		if (afxShellManager == nullptr)
		{
			ASSERT(FALSE);
		}
		else
		{
			bUpdate = afxShellManager->BrowseForFolder(strPath, mWndList, strPath);
		}
	}
	else
	{
		CFileDialog dlg(mOpenFileDialog, mDefaultExtension, strPath, mFileOpenFlags, mFilterString, mWndList);
		if (dlg.DoModal() == IDOK)
		{
			bUpdate = TRUE;
			strPath = dlg.GetPathName();
		}
	}

	if (bUpdate)
	{
		if (mWndInPlace != nullptr)
		{
			mWndInPlace->SetWindowTextW(strPath);
		}

		mValue = (LPCTSTR) strPath;
	}

	mButtonIsDown = FALSE;
	Redraw();

	if (mWndInPlace != nullptr)
	{
		mWndInPlace->SetFocus();
	}
	else
	{
		mWndList->SetFocus();
	}
}

} // namespace hsui
