#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API ToolTipCtrl : public CMFCToolTipCtrl
{
	DECLARE_DYNCREATE(ToolTipCtrl)
	DECLARE_MESSAGE_MAP()

public:
	ToolTipCtrl();
	virtual ~ToolTipCtrl();

protected:
	afx_msg int OnCreate(CREATESTRUCT* createStruct);
	afx_msg void OnShow(NMHDR* nmhdr, LRESULT* result);
	virtual CSize OnDrawLabel(CDC* dc, CRect rect, BOOL calcOnly) override;
};

} // namespace hsui
