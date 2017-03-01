#include "hsmnPrecompiled.h"
#include "hsmnMonitoringPane.h"
#include "hsmnApplication.h"
#include <hsuiVisualManager.h>
#include <hsuiEditCtrl.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(MonitoringPane, DockablePane)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

MonitoringPane::MonitoringPane()
{
}

MonitoringPane::~MonitoringPane()
{
}

int MonitoringPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DockablePane::OnCreate(lpCreateStruct);

	mBrush.CreateSolidBrush(VisualManager::ToGdiColor(VisualManager::CLR_BASE));

	mStart.CreateWnd(this, L"Start", 1, 1, 100, 100);
	mStart.SetMode(ButtonCtrl::MODE_TEXT);
	mStop.CreateWnd(this, L"Stop", 1, 1, 100, 100);
	mStop.SetMode(ButtonCtrl::MODE_TEXT);
	mStop.EnableWindow(FALSE);

	ListCtrl::Column column;
	vector<ListCtrl::Column> columns;

	column.text = L"Name";
	column.width = 300;
	columns.push_back(column);

	column.text = L"Value";
	column.width = 500;
	columns.push_back(column);

	mView.CreateWnd(this, columns, 1, 1, 100, 100);
	mView.ShowHorzScrollBar(false);

	return 0;
}

void MonitoringPane::OnDestroy()
{
	DockablePane::OnDestroy();
}

void MonitoringPane::OnSize(UINT type, int cx, int cy)
{
	DockablePane::OnSize(type, cx, cy);

	static const int CONTROL_AREA_HEIGHT = 32;

	int w = cx - 2;
	int h = cy - CONTROL_AREA_HEIGHT - 1;
	int x = 1;
	int y = CONTROL_AREA_HEIGHT;
	mView.MoveAndResizeWnd(x, y, w, h);

	int col0 = static_cast<int>(w * 0.3);
	int col1 = w - col0 - ScrollBar::FIXED_SIZE - 1;
	mView.SetColumnWidth(0, col0);
	mView.SetColumnWidth(1, col1);

	x = 3;
	y = 5;
	w = (cx - 9) / 2;
	h = EditCtrl::FIXED_HEIGHT;
	mStart.MoveAndResizeWnd(x, y, w, h);
	
	x = cx - 3 - w;
	mStop.MoveAndResizeWnd(x, y, w, h);
}

void MonitoringPane::OnSetFocus(CWnd* pOldWnd)
{
	DockablePane::OnSetFocus(pOldWnd);

	mView.SetFocus();
}

void MonitoringPane::OnPaint()
{
	CPaintDC dc(this);
	
	CRect rect;
	GetClientRect(&rect);

	dc.FillRect(&rect, &mBrush);

	CPen* originalPen = dc.SelectObject(theVisualManager->GetLevel3Pen());

	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.right - 1, rect.top);
	dc.LineTo(rect.right - 1, rect.bottom - 1);
	dc.LineTo(rect.left, rect.bottom - 1);
	dc.LineTo(rect.left, rect.top);

	mView.GetWindowRect(&rect);
	ScreenToClient(&rect);
	dc.MoveTo(rect.left, rect.top - 1);
	dc.LineTo(rect.right, rect.top - 1);

	dc.SelectObject(originalPen);
}

} // namespace hsmn
