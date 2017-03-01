#pragma once

#include <xscD2DRenderer.h>

namespace hsmn {

class InfoGadget : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	InfoGadget();
	virtual ~InfoGadget();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);

	afx_msg int OnSetText(LPCTSTR text);

	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	CString mText;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
};

} // namespace hsmn
