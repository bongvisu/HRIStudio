#include "hsuiPrecompiled.h"
#include "hsuiPropGridColorButton.h"
#include "hsuiPropGridColorBar.h"
#include "hsuiVisualManager.h"

namespace hsui {

IMPLEMENT_SERIAL(PropGridColorButton, CMFCToolBarButton, 1)

PropGridColorButton::PropGridColorButton(COLORREF color, BOOL isAutomatic, BOOL isOther,
	LPCTSTR colorName, BOOL highlight, BOOL isDocument, BOOL isOtherColor)
{
	mColor = color;
	mHighlight = highlight;
	m_strText = (colorName == nullptr) ? _T("") : colorName;
	mIsAutomatic = isAutomatic;
	mIsOther = isOther;
	mIsLabel = FALSE;
	mIsDocument = isDocument;
	mIsOtherColor = isOtherColor;
	mParentBar = nullptr;
	m_bLocked = TRUE;
}

PropGridColorButton::PropGridColorButton(LPCTSTR colorName, BOOL isDocument)
{
	ENSURE(colorName != nullptr);

	mColor = (COLORREF)-1;
	mHighlight = FALSE;
	m_strText = colorName;
	mIsAutomatic = FALSE;
	mIsOther = FALSE;
	mIsLabel = TRUE;
	mIsDocument = isDocument;
	mIsOtherColor = FALSE;
	mParentBar = nullptr;
	m_bLocked = TRUE;
}

void PropGridColorButton::OnDraw(CDC* dc, const CRect& rect, CMFCToolBarImages*,
	BOOL isHorz, BOOL customizeMode, BOOL highlight, BOOL drawBorder, BOOL)
{
	ASSERT_VALID(dc);
	ASSERT_VALID(this);

	CPalette* oldPalette = nullptr;

	if (mParentBar != nullptr)
	{
		oldPalette = mParentBar->SelectPalette(dc);
	}

	BOOL bDisabled = (m_nStyle & TBBS_DISABLED);

	UINT nStyle = m_nStyle;

	if (mHighlight)
	{
		m_nStyle |= TBBS_CHECKED;
	}

	// Fill button interior:
	FillInterior(dc, rect, highlight && !mIsLabel);

	// Draw button border:
	if (!bDisabled && !mIsLabel && HaveHotBorder() && drawBorder && !customizeMode)
	{
		if (m_nStyle &(TBBS_PRESSED | TBBS_CHECKED))
		{
			// Pressed in or checked:
			theVisualManager->OnDrawButtonBorder(dc, this, rect, CMFCVisualManager::ButtonsIsPressed);
		}
		else if (highlight && !(m_nStyle &(TBBS_CHECKED | TBBS_INDETERMINATE)))
		{
			theVisualManager->OnDrawButtonBorder(dc, this, rect, CMFCVisualManager::ButtonsIsHighlighted);
		}
	}

	CRect rectFrame = rect;
	rectFrame.DeflateRect(2, 2);

	if (mIsOther || mIsAutomatic || mIsLabel)
	{
		CRect rectText = rect;

		if (mIsAutomatic && mColor != (COLORREF)-1)
		{
			CRect rectColor = rectFrame;
			rectColor.DeflateRect(3, 3);
			rectColor.right = rectColor.left + rectColor.Height();

			if (!bDisabled)
			{
				CBrush br(PALETTERGB(GetRValue(mColor), GetGValue(mColor), GetBValue(mColor)));
				CPen pen(PS_SOLID, 1, GetGlobalData()->clrBarShadow);

				CBrush* pOldBrush = dc->SelectObject(&br);
				CPen* pOldPen = dc->SelectObject(&pen);

				dc->Rectangle(rectColor);

				dc->SelectObject(pOldPen);
				dc->SelectObject(pOldBrush);
			}
			else
			{
				dc->Draw3dRect(rectColor, GetGlobalData()->clrBarHilite, GetGlobalData()->clrBarShadow);
				rectColor.OffsetRect(1, 1);
				dc->Draw3dRect(rectColor, GetGlobalData()->clrBarShadow, GetGlobalData()->clrBarHilite);
			}

			rectText.left = rectColor.right + 2;
		}

		// Draw label:
		dc->SetTextColor((m_nStyle & TBBS_DISABLED) && !mIsLabel ?
			GetGlobalData()->clrGrayedText : GetGlobalData()->clrBarText);

		UINT nFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
		if (!mIsLabel)
		{
			nFormat |= DT_CENTER;
		}

		CFont* pCurrFont = nullptr;
		if (!isHorz)
		{
			pCurrFont = dc->SelectObject(&(GetGlobalData()->fontRegular));
			ENSURE(pCurrFont != nullptr);
		}

		dc->DrawText(m_strText, &rectText, nFormat);

		if (pCurrFont != nullptr)
		{
			dc->SelectObject(pCurrFont);
		}
	}
	else if (!bDisabled)
	{
		// Draw color box:
		CBrush br(PALETTERGB(GetRValue(mColor), GetGValue(mColor), GetBValue(mColor)));

		CBrush* pOldBrush = dc->SelectObject(&br);
		CPen* pOldPen = dc->SelectObject(&(GetGlobalData()->penBarShadow));

		rectFrame.right--;
		rectFrame.bottom--;

		dc->Rectangle(rectFrame);

		dc->SelectObject(pOldPen);
		dc->SelectObject(pOldBrush);
	}

	// Draw frame:
	if (!mIsOther && !mIsLabel)
	{
		if (!bDisabled)
		{
			dc->Draw3dRect(rectFrame, GetGlobalData()->clrBarShadow, GetGlobalData()->clrBarShadow);
		}
		else
		{
			dc->Draw3dRect(rectFrame, GetGlobalData()->clrBarHilite, GetGlobalData()->clrBarShadow);

			rectFrame.OffsetRect(1, 1);

			dc->Draw3dRect(rectFrame, GetGlobalData()->clrBarShadow, GetGlobalData()->clrBarHilite);
		}
	}

	if (oldPalette != nullptr)
	{
		dc->SelectPalette(oldPalette, FALSE);
	}

	m_nStyle = nStyle;
}

BOOL PropGridColorButton::OnToolHitTest(const CWnd* pWnd, TOOLINFO* pTI)
{
	UNREFERENCED_PARAMETER(pWnd);

	if (m_nStyle & TBBS_DISABLED)
	{
		return FALSE;
	}

	if (!CMFCToolBar::GetShowTooltips() || pTI == nullptr)
	{
		return FALSE;
	}

	CString str = m_strText;
	if (!mIsAutomatic && !mIsOther && !mIsLabel)
	{
		if (!PropGridColorBar::msColorNames.Lookup(mColor, str))
		{
			str.Format(_T("Hex={%02X,%02X,%02X}"), GetRValue(mColor), GetGValue(mColor), GetBValue(mColor));
		}
	}

	pTI->lpszText = (LPTSTR) ::calloc((str.GetLength() + 1), sizeof(TCHAR));
	if (pTI->lpszText == nullptr)
	{
		return FALSE;
	}

	lstrcpy(pTI->lpszText, str);

	return TRUE;
}

void PropGridColorButton::OnChangeParentWnd(CWnd* pWndParent)
{
	CMFCToolBarButton::OnChangeParentWnd(pWndParent);
	mParentBar = DYNAMIC_DOWNCAST(PropGridColorBar, pWndParent);
}

} // namespace hsui
