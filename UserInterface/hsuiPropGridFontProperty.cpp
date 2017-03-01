#include "hsuiPrecompiled.h"
#include "hsuiPropGridFontProperty.h"
#include "hsuiPropGridCtrl.h"
#include "hsuiVisualManager.h"

#undef min
#undef max

namespace hsui {

IMPLEMENT_DYNAMIC(PropGridFontProperty, PropGridProperty)

PropGridFontProperty::PropGridFontProperty(
	const CString& strName,
	LOGFONT& lf,
	DWORD dwFontDialogFlags,
	LPCTSTR lpszDescr,
	DWORD_PTR dwData,
	COLORREF color)
	:
	PropGridProperty(strName, COleVariant((LPCTSTR)lf.lfFaceName), lpszDescr, dwData),
	mFontDialogFlags(dwFontDialogFlags)
{
	mFlags = PROP_HAS_BUTTON;
	mLogFont = lf;
	mOriginalLogFont = lf;
	mAllowEdit = FALSE;
	mColor = color;
}

PropGridFontProperty::~PropGridFontProperty()
{
}

void PropGridFontProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);
	ASSERT_VALID(mWndInPlace);
	ASSERT(::IsWindow(mWndInPlace->GetSafeHwnd()));

	LOGFONT lfPrev = mLogFont;
	COLORREF nColorPrev = mColor;

	mButtonIsDown = TRUE;
	Redraw();

#pragma warning(disable : 4244)

	CFontDialog dlg(&mLogFont, mFontDialogFlags, nullptr, mWndList);

#pragma warning(default : 4244)

	if (mColor != (COLORREF)-1)
	{
		dlg.m_cf.rgbColors = mColor;
	}

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&mLogFont);
		mColor = dlg.GetColor();

		if (memcmp(&lfPrev, &mLogFont, sizeof(lfPrev) - sizeof(lfPrev.lfFaceName)) ||
			_tcscmp( lfPrev.lfFaceName, mLogFont.lfFaceName) || nColorPrev != mColor)
		{
			mWndList->OnPropertyChanged(this);
		}

		if (mWndInPlace != nullptr)
		{
			mWndInPlace->SetWindowText(FormatProperty());
		}
		else
		{
			mValue = (LPCTSTR) FormatProperty();
		}
	}

	if (mWndInPlace != nullptr)
	{
		mWndInPlace->SetFocus();
	}
	else
	{
		mWndList->SetFocus();
	}

	mButtonIsDown = FALSE;
	Redraw();
}

CString PropGridFontProperty::FormatProperty()
{
	CString str;
	CWindowDC dc(mWndList);

	int nLogY = dc.GetDeviceCaps(LOGPIXELSY);
	if (nLogY != 0)
	{
		str.Format( _T("%s(%i)"), mLogFont.lfFaceName, MulDiv(72, -mLogFont.lfHeight, nLogY));
	}
	else
	{
		str = mLogFont.lfFaceName;
	}

	return str;
}

void PropGridFontProperty::ResetOriginalValue()
{
	PropGridProperty::ResetOriginalValue();
	mLogFont = mOriginalLogFont;
}

} // namespace hsui
