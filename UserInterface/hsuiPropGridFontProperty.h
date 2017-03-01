#pragma once

#include <hsuiPropGridProperty.h>

namespace hsui {

class PropGridFontProperty : public PropGridProperty
{
	DECLARE_DYNAMIC(PropGridFontProperty)

public:
	PropGridFontProperty(const CString& name, LOGFONT& lf,
		DWORD fontDialogFlags = CF_EFFECTS | CF_SCREENFONTS, 
		LPCTSTR descr = nullptr, DWORD_PTR data = 0,
		COLORREF color = (COLORREF)-1);
	
	virtual ~PropGridFontProperty();

	virtual void OnClickButton(CPoint point);
	virtual CString FormatProperty();

	LPLOGFONT GetLogFont() { return &mLogFont; }
	COLORREF GetColor() const { return mColor; }

protected:
	virtual BOOL IsValueChanged() const
	{
		return ((memcmp(&mLogFont, &mOriginalLogFont, sizeof(LOGFONT)) != 0)
			|| (lstrcmp(mLogFont.lfFaceName, mOriginalLogFont.lfFaceName) != 0));
	}

	virtual void ResetOriginalValue();

protected:
	LOGFONT mLogFont;
	LOGFONT mOriginalLogFont;
	DWORD_PTR mFontDialogFlags;
	COLORREF mColor;
};

} // namespace hsui
