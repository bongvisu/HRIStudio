#include "hsuiPrecompiled.h"
#include "hsuiDockingTabCtrl.h"

#undef max
#undef min

#define AFX_MIN_SCROLL_WIDTH  (::GetSystemMetrics(SM_CXHSCROLL) * 2)
#define AFX_SPLITTER_WIDTH    5

namespace hsui {

IMPLEMENT_DYNCREATE(DockingTabCtrl, CMFCTabCtrl);

BEGIN_MESSAGE_MAP(DockingTabCtrl, CMFCTabCtrl)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

DockingTabCtrl::DockingTabCtrl()
{
	m_nTabBorderSize = 0;
}

DockingTabCtrl::~DockingTabCtrl()
{
}

void DockingTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CMFCTabCtrl::OnSize(nType, cx, cy);
	Invalidate(FALSE);
}

void DockingTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bResize)
	{
		CRect rectNew = m_rectResizeDrag;
		ClientToScreen(&point);
		CSize size;
		if (m_ResizeMode == RESIZE_VERT)
		{
			int nWidth = rectNew.Width();
			size.cx = size.cy = nWidth;

			rectNew.left = point.x - nWidth / 2;
			rectNew.right = rectNew.left + nWidth;

			if (rectNew.left < m_rectResizeBounds.left)
			{
				rectNew.left = m_rectResizeBounds.left;
				rectNew.right = rectNew.left + nWidth;
			}
			else if (rectNew.right > m_rectResizeBounds.right)
			{
				rectNew.right = m_rectResizeBounds.right;
				rectNew.left = rectNew.right - nWidth;
			}
		}
		else if (m_ResizeMode == RESIZE_HORIZ)
		{
			int nHeight = rectNew.Height();
			size.cx = size.cy = nHeight;

			rectNew.top = point.y - nHeight / 2;
			rectNew.bottom = rectNew.top + nHeight;

			if (rectNew.top < m_rectResizeBounds.top)
			{
				rectNew.top = m_rectResizeBounds.top;
				rectNew.bottom = rectNew.top + nHeight;
			}
			else if (rectNew.bottom > m_rectResizeBounds.bottom)
			{
				rectNew.bottom = m_rectResizeBounds.bottom;
				rectNew.top = rectNew.bottom - nHeight;
			}
		}

		DrawResizeDragRect(rectNew, m_rectResizeDrag);
		m_rectResizeDrag = rectNew;
		return;
	}

	if (m_bTrackSplitter)
	{
		int nSplitterLeftPrev = m_rectTabSplitter.left;

		m_nHorzScrollWidth = std::min(
			m_nScrollBarRight - m_rectTabsArea.left - AFX_SPLITTER_WIDTH,
			m_nScrollBarRight - point.x);
		m_nHorzScrollWidth = std::max(AFX_MIN_SCROLL_WIDTH, m_nHorzScrollWidth);
		AdjustWndScroll();

		if (m_rectTabSplitter.left > nSplitterLeftPrev)
		{
			CRect rect = m_rectTabSplitter;
			rect.left = nSplitterLeftPrev - 20;
			rect.right = m_rectTabSplitter.left;
			rect.InflateRect(0, GetTabBorderSize() + 1);

			InvalidateRect(rect);
		}

		CRect rectTabSplitter = m_rectTabSplitter;
		rectTabSplitter.InflateRect(0, GetTabBorderSize());

		InvalidateRect(rectTabSplitter);
		UpdateWindow();
		AdjustTabsScroll();
	}
	else if (GetCapture() == this && IsMDITabGroup() && m_bReadyToDetach)
	{
		CPoint pointDelta;
		GetCursorPos(&pointDelta);
		pointDelta = m_ptHot - pointDelta;
		int nDrag = GetSystemMetrics(SM_CXDRAG);
		if (GetCapture() == this &&
			m_bReadyToDetach &&
			(abs(pointDelta.x) < nDrag &&
			abs(pointDelta.y) < nDrag))
		{
			return;
		}

		if (GetParent()->SendMessage(
			AFX_WM_ON_TABGROUPMOUSEMOVE, nFlags, MAKELPARAM(point.x, point.y)))
		{
			return;
		}
	}

	if (!m_bFlat)
	{
		if (CMFCVisualManager::GetInstance()->AlwaysHighlight3DTabs())
		{
			m_bHighLightTabs = TRUE;
		}
		else if (m_bIsOneNoteStyle)
		{
			m_bHighLightTabs = CMFCVisualManager::GetInstance()->IsHighlightOneNoteTabs();
		}
	}

	int iPrevHighlighted = m_iHighlighted;
	do
	{
		BOOL bTabCloseButtonHighlighted = m_bTabCloseButtonHighlighted;
		m_bTabCloseButtonHighlighted = m_rectCloseButton.PtInRect(point);

		if (bTabCloseButtonHighlighted != m_bTabCloseButtonHighlighted)
		{
			RedrawWindow(m_rectCloseButton);
		}

		if (m_iHighlighted >= 0 && m_iPressed < 0 && !m_bReadyToDetach)
		{
			CPoint pt = point;
			ClientToScreen(&pt);
			CWnd* pWnd = CWnd::WindowFromPoint(pt);
			if (pWnd != nullptr && pWnd->GetSafeHwnd() != GetSafeHwnd())
			{
				ReleaseCapture();
				m_iHighlighted = -1;
				break;
			}
		}

		m_iHighlighted = GetTabFromPoint(point);

		if (m_iPressed >= 0 && m_iHighlighted != m_iPressed)
		{
			m_iHighlighted = -1;
		}

		if (m_iHighlighted != iPrevHighlighted &&(m_bHighLightTabs || IsActiveTabCloseButton()))
		{
			if (iPrevHighlighted < 0)
			{
				if (m_iHighlighted >= 0)
				{
					SetCapture();
				}
			}
			else
			{
				if (m_iHighlighted < 0 && m_iPressed < 0)
				{
					m_bTabCloseButtonHighlighted = FALSE;
					m_bTabCloseButtonPressed = FALSE;

					if (!m_bReadyToDetach)
					{
						ReleaseCapture();
					}
				}
			}
		}

		if (m_bReadyToDetach)
		{
			int nNumTabs = m_iTabsNum; // how many tabs before detch

			// try to rearrange tabs if their number > 1
			if (IsPtInTabArea(point) && nNumTabs > 1 && m_bEnableTabSwap)
			{
				CRect rectTab;
				int nTabNum = GetTabFromPoint(point);

				if (nTabNum != m_iActiveTab && nTabNum != -1)
				{
					int nSecondTab = m_iActiveTab;
					SwapTabs(nTabNum, nSecondTab);
					RecalcLayout();
					SetActiveTab(nTabNum);
					int nCurrTabNum = GetTabFromPoint(point);
					if (nCurrTabNum != nTabNum)
					{
						GetTabRect(nTabNum, rectTab);
						CPoint ptCursorNewPos = point;

						ptCursorNewPos.x = rectTab.left + m_nOffsetFromTabLeft;

						ClientToScreen(&ptCursorNewPos);
						SetCursorPos(ptCursorNewPos.x, ptCursorNewPos.y);
					}
				}
				break;
			}

			if (IsPtInTabArea(point))
			{
				break;
			}

			BOOL bDetachSucceeded = DetachTab(DM_MOUSE);

			if (bDetachSucceeded && nNumTabs <= 2)
			{
				// last tab was detached successfully - run out, because the control
				// has been destroyed
				break;
			}

			if (bDetachSucceeded)
			{
				m_bReadyToDetach = FALSE;
			}
		}
	} while (false);

	if (iPrevHighlighted != m_iHighlighted)
	{
		Invalidate(FALSE);
	}
}

} // namespace hsui
