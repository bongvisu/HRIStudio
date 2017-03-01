#include "hsuiPrecompiled.h"
#include "hsuiGenericTabCtrl.h"

namespace hsui {

IMPLEMENT_DYNAMIC(GenericTabCtrl, CMFCTabCtrl)

BEGIN_MESSAGE_MAP(GenericTabCtrl, CMFCTabCtrl)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

GenericTabCtrl::GenericTabCtrl()
{
	m_nTabBorderSize = 0;
}

GenericTabCtrl::~GenericTabCtrl()
{
}

void GenericTabCtrl::CreateWnd(CWnd* parent, UINT id, int x, int y, int w, int h)
{
	CRect rect(x, y, x + w, y + h);
	Create(STYLE_3D, rect, parent, id, CMFCBaseTabCtrl::LOCATION_TOP);
}

void GenericTabCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void GenericTabCtrl::ResizeWnd(int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void GenericTabCtrl::MoveAndResizeWnd(int x, int y, int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void GenericTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CMFCTabCtrl::OnSize(nType, cx, cy);
	Invalidate(FALSE);
}

void GenericTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CMFCTabCtrl::OnMouseMove(nFlags, point);
	Invalidate(FALSE);
}

} // namespace hsui
