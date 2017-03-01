#include "hsmnPrecompiled.h"
#include "hsmnLibraryPane.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(LibraryPane, DockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LibraryPane::LibraryPane()
{
}

LibraryPane::~LibraryPane()
{
}

int LibraryPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DockablePane::OnCreate(lpCreateStruct);

	mView.Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, { 0, 0, 8, 8 }, this, 1);

	return 0;
}

void LibraryPane::OnDestroy()
{
	DockablePane::OnDestroy();
}

void LibraryPane::OnSize(UINT nType, int cx, int cy)
{
	DockablePane::OnSize(nType, cx, cy);

	mView.SetWindowPos(nullptr, 1, 1, cx - 2, cy - 2, SWP_NOZORDER | SWP_NOACTIVATE);
}

void LibraryPane::OnSetFocus(CWnd* pOldWnd)
{
	DockablePane::OnSetFocus(pOldWnd);

	mView.SetFocus();
}

void LibraryPane::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	CPen* originalPen = dc.SelectObject(theVisualManager->GetLevel3Pen());

	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.right - 1, rect.top);
	dc.LineTo(rect.right - 1, rect.bottom - 1);
	dc.LineTo(rect.left, rect.bottom - 1);
	dc.LineTo(rect.left, rect.top);

	dc.SelectObject(originalPen);
}

} // namespace hsmn
