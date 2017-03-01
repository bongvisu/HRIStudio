#pragma once

namespace hsui {

class PropGridColorButton : public CMFCToolBarButton
{
	DECLARE_SERIAL(PropGridColorButton)
	friend class PropGridColorBar;

protected:
	PropGridColorButton(COLORREF color = RGB(0, 0, 0), BOOL isAutomatic = FALSE, BOOL other = FALSE,
		LPCTSTR colorName = nullptr, BOOL highlight = FALSE, BOOL isDocument = FALSE, BOOL isOtherColor = FALSE);

	PropGridColorButton(LPCTSTR colorName, BOOL isDocument = FALSE);

	virtual void OnDraw(CDC* dc, const CRect& rect, CMFCToolBarImages* images, BOOL horz = TRUE,
		BOOL customizeMode = FALSE, BOOL highlight = FALSE, BOOL drawBorder = TRUE, BOOL grayDisabledButtons = TRUE);

	virtual BOOL OnToolHitTest(const CWnd* wnd, TOOLINFO* ti);
	virtual void OnChangeParentWnd(CWnd* wndParent);

protected:
	COLORREF mColor;
	BOOL mHighlight;
	BOOL mIsAutomatic;
	BOOL mIsOther;
	BOOL mIsLabel;
	BOOL mIsDocument;
	BOOL mIsOtherColor;
	PropGridColorBar* mParentBar;
};

} // namespace hsui
