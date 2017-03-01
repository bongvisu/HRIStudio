#pragma once

#include <hsuiPropGridProperty.h>

namespace hsui {

class PropGridFileProperty : public PropGridProperty
{
	DECLARE_DYNAMIC(PropGridFileProperty)

public:
	PropGridFileProperty(const CString& name, const CString& folderName,
		DWORD_PTR data = 0, LPCTSTR descr = nullptr);

	PropGridFileProperty(const CString& name, BOOL openFileDialog, const CString& fileName,
		LPCTSTR defExt = nullptr, DWORD flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR filter = nullptr, LPCTSTR descr = nullptr, DWORD_PTR data = 0);

	virtual ~PropGridFileProperty();

	virtual void OnClickButton(CPoint point);

protected:
	BOOL    mIsFolder;

	// File open dialog atributes
	BOOL    mOpenFileDialog; // TRUE - use "File Open/Save" diaog; otherwise - folder selection dialog
	DWORD   mFileOpenFlags;
	CString mDefaultExtension;
	CString mFilterString;
};

} // namespace hsui
