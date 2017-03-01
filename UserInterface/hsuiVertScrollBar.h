#pragma once

#include <hsuiScrollBar.h>

namespace hsui {

class HSUI_API VertScrollBar : public ScrollBar
{
	DECLARE_MESSAGE_MAP()

public:
	VertScrollBar();
	virtual ~VertScrollBar();

	virtual Type GetType() const override;
	virtual void CreateWnd(CWnd* parent, int x, int y, int wh) override;

	virtual void ResizeWnd(int wh) override;
	virtual void MoveAndResizeWnd(int x, int y, int wh) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);

	virtual void UpdateScrollParams() override;
	virtual void UpdateActivityState(CPoint point) override;
	virtual void Render() override;
};

} // namespace hsui
