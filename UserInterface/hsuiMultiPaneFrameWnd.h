#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API MultiPaneFrameWnd : public CMultiPaneFrameWnd
{
	DECLARE_SERIAL(MultiPaneFrameWnd)
	DECLARE_MESSAGE_MAP()

public:
	virtual ~MultiPaneFrameWnd();

protected:
	MultiPaneFrameWnd();

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnNcPaint();
	virtual void OnDrawCaptionButtons(CDC* dc) override;
};

} // namespace hsui
