#pragma once

#include <hsuiComboCtrl.h>
#include <hsuiButtonCtrl.h>

namespace hsmn {

class ToolStrip
	:
	public CMFCToolBar,
	public hsui::ButtonCtrl::IListener,
	public hsui::ComboCtrl::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	ToolStrip();
	virtual ~ToolStrip();

	virtual int GetRowHeight() const override;
	virtual void OnClicked(hsui::ButtonCtrl* buttonCtrl) override;
	virtual void OnSelectionChanged(hsui::ComboCtrl* comboCtrl) override;

protected:
	virtual void DoPaint(CDC* dc) override;
	
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);

	std::vector<int> mSeparators;
	ID2D1SolidColorBrush* mBrush;

	hsui::ButtonCtrl mNew;
	hsui::ButtonCtrl mOpen;
	hsui::ButtonCtrl mClose;

	hsui::ButtonCtrl mSave;

	hsui::ButtonCtrl mCut;
	hsui::ButtonCtrl mCopy;
	hsui::ButtonCtrl mPaste;

	hsui::ButtonCtrl mUndo;
	hsui::ButtonCtrl mRedo;

	hsui::ComboCtrl mSelectionMode;

	hsui::ButtonCtrl mAlignTop;
	hsui::ButtonCtrl mAlignLeft;
	hsui::ButtonCtrl mAlignRight;
	hsui::ButtonCtrl mAlignBottom;

	hsui::ButtonCtrl mSameSize;
	hsui::ButtonCtrl mSameWidth;
	hsui::ButtonCtrl mSameHeight;

	hsui::ButtonCtrl mCompile;
	hsui::ButtonCtrl mDownload;

	hsui::ButtonCtrl mSettings;
	hsui::ButtonCtrl mHelp;
};

} // namespace hsmn
