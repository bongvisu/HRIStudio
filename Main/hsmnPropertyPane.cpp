#include "hsmnPrecompiled.h"
#include "hsmnPropertyPane.h"
#include <hsuiPropManager.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(PropertyPane, DockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

PropertyPane::PropertyPane()
{
	mView = nullptr;
}

PropertyPane::~PropertyPane()
{
}

int PropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DockablePane::OnCreate(lpCreateStruct);

	mView = thePropManager->CreateView(this);

	return 0;
}

void PropertyPane::OnDestroy()
{
	thePropManager->DestroyView();

	DockablePane::OnDestroy();
}

void PropertyPane::OnSize(UINT nType, int cx, int cy)
{
	DockablePane::OnSize(nType, cx, cy);

	mView->SetWindowPos(nullptr, 1, 1, cx - 2, cy - 2, SWP_NOZORDER | SWP_NOACTIVATE);
}

void PropertyPane::OnSetFocus(CWnd* pOldWnd)
{
	DockablePane::OnSetFocus(pOldWnd);

	mView->SetFocus();
}

void PropertyPane::OnPaint()
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
