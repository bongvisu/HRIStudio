#pragma once

#include <hsuiDockablePane.h>
#include <hsuiListCtrl.h>
#include <hsuiButtonCtrl.h>

namespace hsmn {

class MonitoringPane : public hsui::DockablePane
{
	DECLARE_MESSAGE_MAP()

public:
	MonitoringPane();
	virtual ~MonitoringPane();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnPaint();

protected:
	CBrush mBrush;
	hsui::ButtonCtrl mStart;
	hsui::ButtonCtrl mStop;
	hsui::ListCtrl mView;
};

} // namespace hsmn
