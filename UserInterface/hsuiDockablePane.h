#pragma once

#include <hsuiModule.h>

namespace hsui {

// To be used as base class
class HSUI_API DockablePane : public CDockablePane
{
	DECLARE_SERIAL(DockablePane);
	DECLARE_MESSAGE_MAP()

public:
	virtual ~DockablePane();

	void CreateWnd(CWnd* parent, UINT id, const CRect& rect);

	virtual void ShowPane(BOOL show, BOOL delay, BOOL activate/* = TRUE*/) override;
	virtual int GetCaptionHeight() const override;

protected:
	DockablePane();

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	virtual void DrawCaption(CDC* dc, CRect rect) override;
};

} // namespace hsui
