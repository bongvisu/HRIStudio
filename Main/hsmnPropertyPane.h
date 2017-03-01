#pragma once

#include <hsuiDockablePane.h>
#include <hsuiPropGridCtrl.h>

namespace hsmn {

class PropertyPane : public hsui::DockablePane
{
	DECLARE_MESSAGE_MAP()

public:
	PropertyPane();
	virtual ~PropertyPane();

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();

	hsui::PropGridCtrl* mView;
};

} // namespace hsmn
