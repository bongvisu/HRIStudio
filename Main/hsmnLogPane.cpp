#include "hsmnPrecompiled.h"
#include "hsmnLogPane.h"
#include "hsmnApplication.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const UINT WU_EVENT_ENQUEUED = WM_APP + 1;

BEGIN_MESSAGE_MAP(LogPane, DockablePane)
	ON_MESSAGE_VOID(WU_EVENT_ENQUEUED, OnEventEnqueued)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LogPane::LogPane()
{
}

LogPane::~LogPane()
{
}

void LogPane::OnEventEnqueued()
{
	EnterCriticalSection(&mQueueLock);
	vector<LogItem> items(std::move(mQueue));
	LeaveCriticalSection(&mQueueLock);

	for (auto& item : items)
	{
		mTempItem.data = reinterpret_cast<void*>(item.senderID);
		mTempItem.subitems[0].text = std::move(item.senderName);
		mTempItem.subitems[1].text = std::move(item.message);

		mView.AddItem(mTempItem);
	}

	int numItems = mView.GetNumItems();
	int diff = numItems - 100;
	if (diff > 0)
	{
		for (int i = 0; i < diff; i++)
		{
			mView.RemoveItem(0);
		}
	}

	mView.Refresh();
}

void LogPane::EnqueueLogItem(LogItem&& item)
{
	EnterCriticalSection(&mQueueLock);

	bool notPending = mQueue.size() == 0;
	mQueue.push_back(std::move(item));

	LeaveCriticalSection(&mQueueLock);

	if (notPending)
	{
		PostMessageW(WU_EVENT_ENQUEUED);
	}
}

int LogPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DockablePane::OnCreate(lpCreateStruct);

	InitializeCriticalSection(&mQueueLock);

	ListCtrl::Column column;
	vector<ListCtrl::Column> columns;

	column.text = L"Sender";
	column.width = 300;
	columns.push_back(column);

	column.text = L"Message";
	column.width = 500;
	columns.push_back(column);

	mView.CreateWnd(this, columns, 1, 1, 100, 100);
	mView.ShowHorzScrollBar(false);

	UINT imageList[] =
	{
		HSMN_IDR_EVENT_LIST_ICON_INFO,
		HSMN_IDR_EVENT_LIST_ICON_ERROR
	};
	mView.BuildImageList(theApp.m_hInstance, 2, imageList);

	mTempItem.image = 0;
	mTempItem.subitems.resize(2);

	ListCtrl::Subitem subitem;
	
	subitem.text = L"System";
	mTempItem.subitems[0] = subitem;

	subitem.text = L"Initialization complete.";
	mTempItem.subitems[1] = subitem;

	mView.AddItem(mTempItem);

	theLogMediator->SetConsumer(static_cast<ILogConsumer*>(this));

	return 0;
}

void LogPane::OnDestroy()
{
	theLogMediator->ResetConsumer();

	DeleteCriticalSection(&mQueueLock);

	DockablePane::OnDestroy();
}

void LogPane::OnSize(UINT type, int cx, int cy)
{
	DockablePane::OnSize(type, cx, cy);

	int w = cx - 2;
	int h = cy - 2;
	mView.ResizeWnd(w, h);

	int col0 = static_cast<int>(w * 0.3);
	int col1 = w - col0 - ScrollBar::FIXED_SIZE - 1;
	mView.SetColumnWidth(0, col0);
	mView.SetColumnWidth(1, col1);
}

void LogPane::OnSetFocus(CWnd* pOldWnd)
{
	DockablePane::OnSetFocus(pOldWnd);

	mView.SetFocus();
}

void LogPane::OnPaint()
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
