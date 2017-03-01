#pragma once

#include <xscSyncs.h>
#include <hsmoLogMediator.h>
#include <hsuiDockablePane.h>
#include <hsuiListCtrl.h>

namespace hsmn {

class LogPane
	:
	public hsui::DockablePane,
	public hsmo::ILogConsumer
{
	DECLARE_MESSAGE_MAP()

public:
	LogPane();
	virtual ~LogPane();

	virtual void EnqueueLogItem(hsmo::LogItem&& eventItem) override;

protected:
	
	afx_msg void OnEventEnqueued();

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnPaint();

protected:
	hsui::ListCtrl mView;
	hsui::ListCtrl::Item mTempItem;

	CRITICAL_SECTION mQueueLock;
	std::vector<hsmo::LogItem> mQueue;
};

} // namespace hsmn
