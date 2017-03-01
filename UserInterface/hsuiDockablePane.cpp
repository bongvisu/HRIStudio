#include "hsuiPrecompiled.h"
#include "hsuiDockablePane.h"
#include "hsuiTabbedPane.h"
#include "hsuiMultiPaneFrameWnd.h"
#include "hsuiVisualManager.h"

namespace hsui {

IMPLEMENT_SERIAL(DockablePane, CDockablePane, VERSIONABLE_SCHEMA | 2)

BEGIN_MESSAGE_MAP(DockablePane, CDockablePane)
	ON_WM_CREATE()
END_MESSAGE_MAP()

DockablePane::DockablePane()
{
	m_pTabbedControlBarRTC = RUNTIME_CLASS(TabbedPane);
	m_pMiniFrameRTC = RUNTIME_CLASS(MultiPaneFrameWnd);
}

DockablePane::~DockablePane()
{
}

void DockablePane::ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate)
{
	CDockablePane::ShowPane(bShow, bDelay, bActivate);
}

void DockablePane::CreateWnd(CWnd* parent, UINT id, const CRect& rect)
{
	assert(GetSafeHwnd() == nullptr);

	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;
	DWORD tabbedStyle = AFX_CBRS_REGULAR_TABS;
	DWORD controlbarStyle = AFX_CBRS_FLOAT | AFX_CBRS_RESIZE | AFX_CBRS_CLOSE;

	Create(nullptr, parent, rect, TRUE, id, style, tabbedStyle, controlbarStyle);
}

int DockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDockablePane::OnCreate(lpCreateStruct);

	SetTabbedPaneRTC(RUNTIME_CLASS(TabbedPane));
	SetMiniFrameRTC(RUNTIME_CLASS(MultiPaneFrameWnd));

	return 0;
}

int DockablePane::GetCaptionHeight() const
{
	if (CDockablePane::GetCaptionHeight())
	{
		return 25;
	}
	return 0;
}

void DockablePane::DrawCaption(CDC* pDC, CRect rectCaption)
{
	CRect rcbtnRight = CRect(rectCaption.BottomRight(), CSize(0, 0));
	int i = 0;
	
	for (i = (int)m_arrButtons.GetUpperBound(); i >= 0; i--)
	{
		if (!m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
		{
			rcbtnRight = m_arrButtons [i]->GetRect();
			break;
		}
	}

	CRect rcbtnLeft = CRect(rectCaption.TopLeft(), CSize(0, 0));
	for (i = (int)m_arrButtons.GetUpperBound(); i >= 0; i--)
	{
		if (m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
		{
			rcbtnLeft = m_arrButtons [i]->GetRect();
			break;
		}
	}

	COLORREF clrCptnText =
		theVisualManager->OnDrawPaneCaption(pDC, this, m_bActive, rectCaption, rcbtnRight);

	for (i = 0; i < m_arrButtons.GetSize(); i ++)
	{
		CMFCCaptionButton* pbtn = m_arrButtons [i];
		ASSERT_VALID(pbtn);

		pbtn->m_clrForeground = clrCptnText;
	}

	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldText = pDC->SetTextColor(clrCptnText);

	CFont* pOldFont = pDC->SelectObject(&(GetGlobalData()->fontRegular));
	ENSURE(pOldFont != nullptr);

	CString strTitle;
	GetWindowText(strTitle);

	rectCaption.right = rcbtnRight.left;
	rectCaption.left = rcbtnLeft.right;
	rectCaption.top--;
	rectCaption.DeflateRect(4, 0);

	pDC->DrawText(strTitle, rectCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

#if 0
	CSize textExtent = pDC->GetTextExtent(strTitle);
	int decoStart = rectCaption.left + textExtent.cx + 6;
	int decoEnd = rectCaption.right;
	int y1 = (rectCaption.Height() - 16) / 2 + 5;
	int y2 = y1 + 4;
	int y3 = y1 + 2;
	for (int x = decoStart; x < decoEnd; ++x)
	{
		int period = (x - decoStart) % 4;
		if (period == 0)
		{
			if (m_bActive)
			{
				pDC->SetPixel(x, y1, VisualManager::GDICLR_LEVEL5);
				pDC->SetPixel(x, y2, VisualManager::GDICLR_LEVEL5);
			}
			else
			{
				pDC->SetPixel(x, y1, VisualManager::GDICLR_LEVEL4);
				pDC->SetPixel(x, y2, VisualManager::GDICLR_LEVEL4);
			}
		}
		else if (period == 1)
		{
			// Do nothing.
		}
		else if (period == 2)
		{
			if (m_bActive)
			{
				pDC->SetPixel(x, y3, VisualManager::GDICLR_LEVEL5);
			}
			else
			{
				pDC->SetPixel(x, y3, VisualManager::GDICLR_LEVEL4);
			}
		}
		else
		{
			// Do nothing.
		}
	}
#endif

	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);
	pDC->SetTextColor(clrOldText);
}

} // namespace hsui
