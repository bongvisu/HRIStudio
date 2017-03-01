#include "hsuiPrecompiled.h"
#include "hsuiMultiPaneFrameWnd.h"
#include "hsuiVisualManager.h"

#undef max
#undef min

namespace hsui {

IMPLEMENT_SERIAL(MultiPaneFrameWnd, CMultiPaneFrameWnd, VERSIONABLE_SCHEMA | 2)

BEGIN_MESSAGE_MAP(MultiPaneFrameWnd, CMultiPaneFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

MultiPaneFrameWnd::MultiPaneFrameWnd()
{
}

MultiPaneFrameWnd::~MultiPaneFrameWnd()
{
}

int MultiPaneFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMultiPaneFrameWnd::OnCreate(lpCreateStruct);

	return 0;
}

void MultiPaneFrameWnd::OnDrawCaptionButtons(CDC* dc)
{
	ASSERT_VALID(dc);

	// Paint caption buttons:
	for (POSITION pos = m_lstCaptionButtons.GetHeadPosition(); pos != nullptr; /**/)
	{
		CMFCCaptionButton* button = (CMFCCaptionButton*)m_lstCaptionButtons.GetNext(pos);
		ASSERT_VALID(button);

		BOOL maximized = TRUE;
		if (button->GetHit() == HTMAXBUTTON && m_bPinned)
		{
			maximized = FALSE;
		}

		button->m_bEnabled =
			(!CMFCToolBar::IsCustomizeMode() ||
			(button->GetHit() == HTCLOSE) ||
			(button->GetHit() == AFX_HTCLOSE));

		button->OnDraw(dc, m_bActive, TRUE, maximized, !button->m_bEnabled);
		button->m_clrForeground = (COLORREF)-1;
	}
}

void MultiPaneFrameWnd::OnNcPaint()
{
	CDockingManager* pDockManager = m_pDockManager != nullptr ?
		m_pDockManager : afxGlobalUtils.GetDockingManager(GetParent());
	if (pDockManager == nullptr)
	{
		return;
	}

	if (pDockManager->m_bLockUpdate)
	{
		return;
	}

	CWindowDC dc(this); // device context for painting

	CDC* pDC = &dc;
	BOOL m_bMemDC = FALSE;
	CDC dcMem;
	CBitmap bmp;
	CBitmap* pOldBmp = nullptr;

	CRect rectWindow;
	GetWindowRect(rectWindow);
	CRect rect;
	rect.SetRect(0, 0, rectWindow.Width(), rectWindow.Height());

	if (dcMem.CreateCompatibleDC(&dc) &&
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
	{
		// Off-screen DC successfully created. Better paint to it then!
		m_bMemDC = TRUE;
		pOldBmp = dcMem.SelectObject(&bmp);
		pDC = &dcMem;
	}

	// client area is not our bussiness :)
	CRect rcClient, rcBar;
	GetWindowRect(rcBar);

	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	rcClient.OffsetRect(-rcBar.TopLeft());

	dc.ExcludeClipRect(rcClient);

	CRgn rgn;
	if (!m_rectRedraw.IsRectEmpty())
	{
		rgn.CreateRectRgnIndirect(m_rectRedraw);
		dc.SelectClipRgn(&rgn);
	}

	// draw border
	OnDrawBorder(pDC);

	CRect rectCaption;
	GetCaptionRect(rectCaption);
	
	if (pDockManager->m_bLockUpdate)
	{
		rectCaption.SetRectEmpty();
	}

	// draw caption:
	GetCaptionRect(rectCaption);

	COLORREF clrText = theVisualManager->OnFillMiniFrameCaption(pDC, rectCaption, this, m_bActive);

	int xBtnsLeft = -1;
	int xBtnsRight = -1;
	for (POSITION pos = m_lstCaptionButtons.GetHeadPosition(); pos != nullptr; /**/)
	{
		CMFCCaptionButton* pBtn = static_cast<CMFCCaptionButton*>(m_lstCaptionButtons.GetNext(pos));
		ASSERT_VALID(pBtn);

		pBtn->m_clrForeground = clrText;

		if (!pBtn->m_bHidden)
		{
			if (pBtn->m_bLeftAlign)
			{
				if (xBtnsRight == -1)
				{
					xBtnsRight = pBtn->GetRect().right + 2;
				}
				else
				{
					xBtnsRight = std::max(xBtnsRight, (int)pBtn->GetRect().right + 2);
				}
			}
			else
			{
				if (xBtnsLeft == -1)
				{
					xBtnsLeft = pBtn->GetRect().left;
				}
				else
				{
					xBtnsLeft = std::min(xBtnsLeft, (int)pBtn->GetRect().left);
				}
			}
		}
	}

	// Paint caption text:
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(clrText);

	CFont* pOldFont = pDC->SelectObject(&(GetGlobalData()->fontBold));
	ASSERT_VALID(pOldFont);

	CString strCaption = GetCaptionText();

	CRect rectText = rectCaption;
	if (xBtnsLeft != -1)
	{
		rectText.right = xBtnsLeft;
	}
	if (xBtnsRight != -1)
	{
		rectText.left = xBtnsRight;
	}

	if (m_barContainerManager.GetVisiblePaneCount() != 1)
	{
		strCaption = theVisualManager->GetAppTitle();
	}

	rectText.DeflateRect(2, 0);
	rectText.top -= 1;
	rectText.bottom -= 1;
	pDC->DrawText(strCaption, rectText, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

	pDC->SelectObject(pOldFont);
	pDC->SelectClipRgn(nullptr);

	// Paint caption buttons:
	OnDrawCaptionButtons(pDC);

	if (m_bMemDC)
	{
		// Copy the results to the on-screen DC:
		CRect rectClip;
		int nClipType = dc.GetClipBox(rectClip);
		if (nClipType != NULLREGION)
		{
			if (nClipType != SIMPLEREGION)
			{
				rectClip = rect;
			}

			dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(),
				rectClip.Height(), &dcMem, rectClip.left, rectClip.top, SRCCOPY);
		}

		dcMem.SelectObject(pOldBmp);
	}

	//CWnd::OnNcPaint();
}

} // namespace hsui
