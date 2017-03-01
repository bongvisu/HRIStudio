#pragma once

#include <hsmoModel.h>
#include <hsuiSystemButtons.h>
#include <hsuiLabelCtrl.h>
#include <hsuiComboCtrl.h>
#include <hsuiCodeEditor.h>
#include <hsuiButtonCtrl.h>

namespace hsmn {

class TagAuthorView
	:
	public CWnd,
	public hsui::ButtonCtrl::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	TagAuthorView();
	virtual ~TagAuthorView();

	void CreateWnd(CWnd* owner);

	virtual void OnClicked(hsui::ButtonCtrl* buttonCtrl) override;

protected:
	afx_msg BOOL OnNcActivate(BOOL);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	virtual void PostNcDestroy() override;
	afx_msg void OnClose();
	afx_msg void OnSize(UINT type, int cx, int cy);

	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap1* mIcon;

	hsui::SystemButtonClose mSysCloseButton;

	hsui::LabelCtrl mLabel1;
	hsui::ComboCtrl mCombo1;

	hsui::LabelCtrl mLabel2;
	hsui::ComboCtrl mCombo2;

	hsui::LabelCtrl mLabel3;
	hsui::ComboCtrl mCombo3;

	hsui::ButtonCtrl mGenerate;
	hsui::ButtonCtrl mCopy;

	hsui::CodeEditor mEdit;

	BOOL mActive;
};

} // namespace hsmn
