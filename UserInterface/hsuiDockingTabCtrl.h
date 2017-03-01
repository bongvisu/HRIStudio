#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API DockingTabCtrl final : public CMFCTabCtrl
{
	DECLARE_DYNCREATE(DockingTabCtrl)
	DECLARE_MESSAGE_MAP()

public:
	virtual ~DockingTabCtrl();

protected:
	DockingTabCtrl();

	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
};

} // namespace hsui
