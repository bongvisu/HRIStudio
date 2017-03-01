#include "hsuiPrecompiled.h"
#include "hsuiTabbedPane.h"
#include "hsuiMultiPaneFrameWnd.h"
#include "hsuiVisualManager.h"

namespace hsui {

IMPLEMENT_SERIAL(TabbedPane, CTabbedPane, VERSIONABLE_SCHEMA | 2)

BEGIN_MESSAGE_MAP(TabbedPane, CTabbedPane)
	ON_WM_CREATE()
END_MESSAGE_MAP()

TabbedPane::TabbedPane()
{
	m_pTabWndRTC = RUNTIME_CLASS(DockingTabCtrl);
	m_pMiniFrameRTC = RUNTIME_CLASS(MultiPaneFrameWnd);
}

TabbedPane::~TabbedPane()
{
}

int TabbedPane::GetCaptionHeight() const
{
	if (CTabbedPane::GetCaptionHeight())
	{
		return 25;
	}
	return 0;
}

int TabbedPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bTabsAlwaysTop = false;
	CTabbedPane::OnCreate(lpCreateStruct);

	m_pTabWndRTC = RUNTIME_CLASS(DockingTabCtrl);
	m_pMiniFrameRTC = RUNTIME_CLASS(MultiPaneFrameWnd);

	return 0;
}

void TabbedPane::DrawCaption(CDC* pDC, CRect rectCaption)
{
	CRect rcbtnRight = CRect(rectCaption.BottomRight(), CSize(0, 0));
	int i = 0;

	for (i = static_cast<int>(m_arrButtons.GetUpperBound()); i >= 0; i --)
	{
		if (!m_arrButtons [i]->m_bLeftAlign && !m_arrButtons [i]->m_bHidden)
		{
			rcbtnRight = m_arrButtons [i]->GetRect();
			break;
		}
	}

	CRect rcbtnLeft = CRect(rectCaption.TopLeft(), CSize(0, 0));
	for (i = static_cast<int>(m_arrButtons.GetUpperBound()); i >= 0; i --)
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

	pDC->SelectObject(pOldFont);
	pDC->SetBkMode(nOldBkMode);
	pDC->SetTextColor(clrOldText);
}

} // namespace hsui
