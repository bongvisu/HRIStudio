#include "hsuiPrecompiled.h"
#include "hsuiPropGridColorProperty.h"
#include "hsuiPropGridCtrl.h"
#include "hsuiVisualManager.h"

#undef min
#undef max

namespace hsui {

IMPLEMENT_DYNAMIC(PropGridColorProperty, PropGridProperty)

PropGridColorProperty::PropGridColorProperty(
	const CString& strName,
	const COLORREF& color,
	CPalette* pPalette,
	LPCTSTR lpszDescr,
	DWORD_PTR dwData)
	:
	PropGridProperty(strName, COleVariant(), lpszDescr, dwData),
	mColor(color),
	mColorOrig(color)
{
	PropGridColorBar::InitColors(pPalette, mColors);

	mValue = (LONG)color;
	mOriginalValue = (LONG)color;

	mFlags = PROP_HAS_LIST;

	mPopup = nullptr;
	mUseStdColorDlg = FALSE;
	mColorAutomatic = RGB(0, 0, 0);
	mColumnsNumber = 5;
}

PropGridColorProperty::~PropGridColorProperty()
{
}

void PropGridColorProperty::OnDrawValue(CDC* pDC, CRect rect)
{
	CRect rectColor = rect;

	rect.left += rect.Height();
	PropGridProperty::OnDrawValue(pDC, rect);

	rectColor.right = rectColor.left + rectColor.Height();
	rectColor.DeflateRect(1, 1);
	rectColor.top++;
	rectColor.left++;

	CBrush br(mColor == (COLORREF)-1 ? mColorAutomatic : mColor);
	pDC->FillRect(rectColor, &br);
	pDC->Draw3dRect(rectColor, 0, 0);
}

void PropGridColorProperty::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	mButtonIsDown = TRUE;
	Redraw();

	CList<COLORREF,COLORREF> lstDocColors;

	mPopup = new PropGridColorPopupMenu(
		mColors,
		mColor,
		nullptr,
		nullptr,
		nullptr,
		lstDocColors,
		mColumnsNumber,
		mColorAutomatic);
	mPopup->SetPropList(mWndList);

	if (!mOtherColorString.IsEmpty()) // Other color button
	{
		mPopup->mColorBar.EnableOtherButton(mOtherColorString, !mUseStdColorDlg);
	}

	if (!mAutoColorString.IsEmpty()) // Automatic color button
	{
		mPopup->mColorBar.EnableAutomaticButton(mAutoColorString, mColorAutomatic);
	}

	CPoint pt;
	pt.x = mWndList->mRectList.left + mWndList->mLeftColumnWidth + 1;
	pt.y = mRectButton.bottom + 1;
	mWndList->ClientToScreen(&pt);

	if (!mPopup->Create(mWndList, pt.x, pt.y, nullptr, FALSE))
	{
		ASSERT(FALSE);
		mPopup = nullptr;
	}
	else
	{
		mPopup->GetMenuBar()->SetFocus();
	}
}

BOOL PropGridColorProperty::OnEdit(LPPOINT /*lptClick*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	mWndInPlace = nullptr;

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect(rectEdit, rectSpin);

	CMFCMaskedEdit* pWndEdit = new CMFCMaskedEdit;
	DWORD dwStyle = WS_VISIBLE | WS_CHILD;

	if (!mEnabled)
	{
		dwStyle |= ES_READONLY;
	}

	pWndEdit->SetValidChars(_T("01234567890ABCDEFabcdef"));

	pWndEdit->Create(dwStyle, rectEdit, mWndList, PropGridCtrl::WND_ID_INPLACE);
	mWndInPlace = pWndEdit;

	mWndInPlace->SetWindowText(FormatProperty());

	mWndInPlace->SetFont(IsModified() && mWndList->mMarkModifiedProperties ?
		&mWndList->mFontBold : mWndList->GetFont());
	mWndInPlace->SetFocus();

	mInPlaceEditing = TRUE;
	return TRUE;
}

void PropGridColorProperty::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mWndList);

	rectSpin.SetRectEmpty();

	rectEdit = mRect;
	//rectEdit.DeflateRect(0, 2);
	rectEdit.top += 3; // XS-NOTE: to look the same as in the display mode

	int nMargin = mWndList->mMarkModifiedProperties && mIsModified ?
		mWndList->mBoldEditLeftMargin : mWndList->mEditLeftMargin;

	rectEdit.left = mWndList->mRectList.left +
		mWndList->mLeftColumnWidth + mRect.Height() + AFX_TEXT_MARGIN - nMargin + 1;

	AdjustButtonRect();
	rectEdit.right = mRectButton.left;
}

void PropGridColorProperty::ResetOriginalValue()
{
	PropGridProperty::ResetOriginalValue();
	mColor = mColorOrig;
}

CString PropGridColorProperty::FormatProperty()
{
	ASSERT_VALID(this);

	if (mColor == (COLORREF)-1)
	{
		return mAutoColorString;
	}

	CString str;
	str.Format(_T("%02x%02x%02x"), GetRValue(mColor), GetGValue(mColor), GetBValue(mColor));

	return str;
}

void PropGridColorProperty::SetColor(COLORREF color)
{
	ASSERT_VALID(this);

	mColor = color;
	mValue = (LONG)color;

	if (::IsWindow(mWndList->GetSafeHwnd()))
	{
		CRect rect = mRect;
		rect.DeflateRect(0, 1);

		mWndList->InvalidateRect(rect);
		mWndList->UpdateWindow();
	}

	if (mWndInPlace != nullptr)
	{
		ASSERT_VALID(mWndInPlace);
		mWndInPlace->SetWindowText(FormatProperty());
	}
}

void PropGridColorProperty::SetColumnsNumber(int nColumnsNumber)
{
	ASSERT_VALID(this);
	ASSERT(nColumnsNumber > 0);

	mColumnsNumber = nColumnsNumber;
}

void PropGridColorProperty::EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable)
{
	ASSERT_VALID(this);

	mColorAutomatic = colorAutomatic;
	mAutoColorString = (!bEnable || lpszLabel == nullptr) ? _T("") : lpszLabel;
}

void PropGridColorProperty::EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg, BOOL bEnable)
{
	ASSERT_VALID(this);

	mUseStdColorDlg = !bAltColorDlg;
	mOtherColorString = (!bEnable || lpszLabel == nullptr) ? _T("") : lpszLabel;
}

BOOL PropGridColorProperty::OnUpdateValue()
{
	ASSERT_VALID(this);

	if (mWndInPlace == nullptr)
	{
		return FALSE;
	}

	ASSERT_VALID(mWndInPlace);
	ASSERT(::IsWindow(mWndInPlace->GetSafeHwnd()));

	CString strText;
	mWndInPlace->GetWindowText(strText);

	COLORREF colorCurr = mColor;

	if (!strText.IsEmpty())
	{
		int nR = 0, nG = 0, nB = 0;
		_stscanf_s(strText, _T("%2x%2x%2x"), &nR, &nG, &nB);
		mColor = RGB(nR, nG, nB);
	}

	if (colorCurr != mColor)
	{
		mWndList->OnPropertyChanged(this);
	}

	return TRUE;
}

} // namespace hsui
