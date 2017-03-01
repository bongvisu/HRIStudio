#pragma once

#include <hsuiPropGridProperty.h>
#include <hsuiPropGridColorPopupMenu.h>

namespace hsui {

class PropGridColorProperty : public PropGridProperty
{
	DECLARE_DYNAMIC(PropGridColorProperty)
	friend class PropGridCtrl;

public:
	PropGridColorProperty(
		const CString& name,
		const COLORREF& color,
		CPalette* palette = nullptr,
		LPCTSTR desc = nullptr,
		DWORD_PTR data = 0);

	virtual ~PropGridColorProperty();

public:
	virtual void OnDrawValue(CDC* dc, CRect rect);
	virtual void OnClickButton(CPoint point);
	virtual BOOL OnEdit(LPPOINT clickPoint);
	virtual BOOL OnUpdateValue();
	virtual CString FormatProperty();

protected:
	virtual BOOL OnKillFocus(CWnd* newWnd)
	{
		return newWnd->GetSafeHwnd() != mPopup->GetSafeHwnd();
	}

	virtual BOOL OnEditKillFocus() { return mPopup == nullptr; }
	virtual BOOL IsValueChanged() const { return mColor != mColorOrig; }

	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);
	virtual void ResetOriginalValue();

public:
	COLORREF GetColor() const { return mColor; }
	void SetColor(COLORREF color);

	// Color popup attributes:
	void EnableAutomaticButton(LPCTSTR label, COLORREF colorAutomatic, BOOL enable = TRUE);
	void EnableOtherButton(LPCTSTR label, BOOL altColorDlg = TRUE, BOOL enable = TRUE);
	void SetColumnsNumber(int numColumns);

protected:
	COLORREF mColor;			// Color value
	COLORREF mColorOrig;		// Original color value
	COLORREF mColorAutomatic;	// Automatic (default) color value
	CString mAutoColorString;	// Atomatic color label
	CString	mOtherColorString;	// Alternative color label
	PropGridColorPopupMenu* mPopup;

	CArray<COLORREF, COLORREF> mColors;

	int  mColumnsNumber; // Number of columns in dropped-down colors list
	BOOL mUseStdColorDlg; // Use standard Windows color dialog
};

} // namespace hsui
