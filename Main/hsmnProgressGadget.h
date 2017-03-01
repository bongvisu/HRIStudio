#pragma once

#include <xscD2DRenderer.h>
#include <hsmoModelManager.h>

namespace hsmn {

class ProgressGadget
	:
	public CWnd,
	public hsmo::IModelManagerObserver
{
	DECLARE_MESSAGE_MAP()

public:
	ProgressGadget();
	virtual ~ProgressGadget();

	virtual void OnCodeTransferStarting() override;
	virtual void OnCodeTransferCompleted() override;

	virtual void OnMonitorConnected() override;
	virtual void OnMonitorDisconnected() override;
	virtual void OnMonitorStateUpdated() override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR timerID);

protected:
	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
};

} // namespace hsmn
