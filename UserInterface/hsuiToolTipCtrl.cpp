#include "hsuiPrecompiled.h"
#include "hsuiToolTipCtrl.h"
#include "hsuiVisualManager.h"

namespace hsui {

static UINT const DELAYED_REDRAW_TIMER = 1000;

IMPLEMENT_DYNCREATE(ToolTipCtrl, CMFCToolTipCtrl)

BEGIN_MESSAGE_MAP(ToolTipCtrl, CMFCToolTipCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TTN_SHOW, OnShow)
END_MESSAGE_MAP()

ToolTipCtrl::ToolTipCtrl()
{
}

ToolTipCtrl::~ToolTipCtrl()
{
}

int ToolTipCtrl::OnCreate(CREATESTRUCT* createStruct)
{
	CMFCToolTipCtrl::OnCreate(createStruct);

	auto classStyle = ::GetClassLongPtr(m_hWnd, GCL_STYLE);
	if (classStyle & CS_DROPSHADOW)
	{
	}

	return 0;
}

void ToolTipCtrl::OnShow(NMHDR* nmhdr, LRESULT* result)
{
	*result = TRUE;

	CPoint ptCursor;
	::GetCursorPos(&ptCursor);

	GetHotButton();

	m_sizeImage = m_Params.m_bDrawIcon ? GetIconSize() : CSize(0, 0);
	m_ptMargin = CPoint(6, 4);

	CRect rectMargin;
	GetMargin(rectMargin);

	CRect rectText;
	GetClientRect(rectText);

	CClientDC dc(this);
	CSize sizeText = OnDrawLabel(&dc, rectText, TRUE);

	int cx = sizeText.cx;
	int cy = sizeText.cy;

	CSize sizeDescr(0, 0);

	if (!m_Params.m_bDrawDescription || m_strDescription.IsEmpty())
	{
		cy = std::max<long>(cy, m_sizeImage.cy);
	}
	else
	{
		sizeDescr = OnDrawDescription(&dc, rectText, TRUE);

		cy += sizeDescr.cy + 2 * m_ptMargin.y;
		cx = std::max<long>(cx, sizeDescr.cx);
		cy = std::max<long>(cy, m_sizeImage.cy);
	}

	if (m_sizeImage.cx > 0 && m_Params.m_bDrawIcon)
	{
		cx += m_sizeImage.cx + m_ptMargin.x;
	}

	cx += 2 * m_ptMargin.x;
	cy += 2 * m_ptMargin.y;

	const int nFixedWidth = GetFixedWidth();
	if (nFixedWidth > 0 && sizeDescr != CSize(0, 0))
	{
		cx = std::max<long>(cx, nFixedWidth);
	}

	CRect rectWindow;
	GetWindowRect(rectWindow);

	int x = rectWindow.left;
	int y = rectWindow.top;

	if (m_ptLocation != CPoint(-1, -1))
	{
		x = m_ptLocation.x;
		y = m_ptLocation.y;
	}

	CRect rectScreen;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfo(MonitorFromPoint(rectWindow.TopLeft(), MONITOR_DEFAULTTONEAREST), &mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
	}

	bool yChanged = false;
	int nBottom = std::max<long>(ptCursor.y + cy + ::GetSystemMetrics(SM_CYCURSOR), y + cy + 2);
	if (nBottom > rectScreen.bottom)
	{
		y = ptCursor.y - cy - 1;

		if (m_pRibbonButton != nullptr && m_ptLocation != CPoint(-1, -1))
		{
			ASSERT_VALID(m_pRibbonButton);

			CMFCRibbonBar* pRibbon = m_pRibbonButton->GetTopLevelRibbonBar();
			if (pRibbon->GetSafeHwnd() != nullptr)
			{
				CRect rectRibbon;
				pRibbon->GetWindowRect(rectRibbon);

				y = rectRibbon.top - cy;
			}
		}

		bool yChanged = true;
	}

	if (x + cx + 2 > rectScreen.right)
	{
		if (yChanged)
		{
			x = ptCursor.x - cx - 1;
		}
		else
		{
			x = rectScreen.right - cx - 1;
		}
	}

	m_sizeCornerRadius = CSize(0, 0);

	RECT rc{ x, y, x + cx, y + cy };
	AdjustRect(&rc, FALSE);

	::SetWindowPos(m_hWnd, HWND_TOPMOST, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

CSize ToolTipCtrl::OnDrawLabel(CDC* dc, CRect rect, BOOL calcOnly)
{
	if (calcOnly)
	{
		return CMFCToolTipCtrl::OnDrawLabel(dc, rect, calcOnly);
	}

	CSize sizeText(0, 0);

	CString strText;
	GetWindowText(strText);

	strText.Replace(_T("\t"), _T("    "));

	BOOL bDrawDescr = m_Params.m_bDrawDescription && !m_strDescription.IsEmpty();

	CFont* pOldFont = (CFont*)dc->SelectObject(
		m_Params.m_bBoldLabel && bDrawDescr ? &(GetGlobalData()->fontBold) : &(GetGlobalData()->fontTooltip));

	if (strText.Find(_T('\n')) >= 0)
	{
		UINT nFormat = DT_NOPREFIX;
		if (m_pRibbonButton != nullptr)
		{
			nFormat |= DT_NOPREFIX;
		}

		rect.OffsetRect(0, -1);
		int nHeight = dc->DrawText(strText, rect, nFormat);
		sizeText = CSize(rect.Width(), nHeight);
	}
	else
	{
		UINT nFormat = DT_CENTER | DT_NOCLIP | DT_SINGLELINE;

		if (!bDrawDescr)
		{
			nFormat |= DT_VCENTER;
		}

		if (m_pRibbonButton != nullptr)
		{
			nFormat |= DT_NOPREFIX;
		}

		rect.OffsetRect(0, -1);
		sizeText.cy = dc->DrawText(strText, rect, nFormat);
		sizeText.cx = rect.Width();
	}

	dc->SelectObject(pOldFont);

	return sizeText;
}

} // namespace hsui
