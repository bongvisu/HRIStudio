#pragma once

#include <hsuiDockablePane.h>
#include "hsmnLibraryView.h"

namespace hsmn {

class LibraryPane : public hsui::DockablePane
{
	DECLARE_MESSAGE_MAP()

public:
	LibraryPane();
	virtual ~LibraryPane();

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();

	LibraryView mView;
};

} // namespace hsmn
