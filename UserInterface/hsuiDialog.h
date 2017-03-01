#pragma once

#include <hsuiSystemButtons.h>
#include <hsuiShadowBorder.h>
#include <hsuiIApplication.h>

namespace hsui {

class HSUI_API Dialog : public CDialogEx
{
	DECLARE_MESSAGE_MAP()

public:
	virtual ~Dialog();

protected:
	Dialog();
	
	virtual void InitWindow() = 0;

	afx_msg void OnWindowPosChanged(WINDOWPOS* windowPos);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnPaint();

	virtual void PaintContent(CDC* dc);

protected:
	hsui::ShadowBorder mShadowBorders[ShadowBorder::NUMBER_OF_ROLES];

	IApplication* mPresentationUpdater;

	ID2D1Bitmap* mIcon;
	ID2D1SolidColorBrush* mBrush;
	SystemButtonClose mSysCloseButton;

	CPen mBorderPenOuter;
	CPen mBorderPenInner;

	int mW, mH;

protected:
	static const int FIXED_BORDER_SIZE;
	static const int FIXED_CAPTION_HEIGHT;
};

} // namespace hsui
