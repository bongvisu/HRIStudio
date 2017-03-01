#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API GenericTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(GenericTabCtrl)
	DECLARE_MESSAGE_MAP()

public:
	GenericTabCtrl();
	virtual ~GenericTabCtrl();

	void CreateWnd(CWnd* parent, UINT id, int x, int y, int w, int h);
	
	void MoveWnd(int x, int y);
	void ResizeWnd(int w, int h);
	void MoveAndResizeWnd(int x, int y, int w, int h);

#if 0
	virtual void RecalcLayout() override;
	virtual void OnDraw(CDC* pDC) override;
#endif

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

} // namespace hsui
