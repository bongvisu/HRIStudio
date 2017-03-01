#pragma once

#include <hsuiDockingTabCtrl.h>

namespace hsui {

class HSUI_API TabbedPane : public CTabbedPane
{
	DECLARE_SERIAL(TabbedPane)
	DECLARE_MESSAGE_MAP()

public:
	virtual ~TabbedPane();

	virtual int GetCaptionHeight() const override;

protected:
	TabbedPane();

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	virtual void DrawCaption(CDC* dc, CRect rectCaption) override;
};

} // namespace hsui
