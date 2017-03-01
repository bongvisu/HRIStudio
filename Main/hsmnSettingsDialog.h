#pragma once

#include <hsuiDialog.h>
#include <hsuiButtonCtrl.h>
#include <hsuiLabelCtrl.h>
#include <hsuiEditCtrl.h>

namespace hsmn {

class SettingsDialog
	:
	public hsui::Dialog,
	public hsui::ButtonCtrl::IListener
{
public:
	SettingsDialog();
	virtual ~SettingsDialog();

protected:
	virtual void OnOK() override;
	virtual void InitWindow() override;
	virtual void OnClicked(hsui::ButtonCtrl* buttonCtrl) override;

protected:
	hsui::LabelCtrl mIPLabel;
	hsui::EditCtrl mIPEdit;
	hsui::LabelCtrl mPortLabel;
	hsui::EditCtrl mPortEdit;

	hsui::LabelCtrl mIDLabel;
	hsui::EditCtrl mIDEdit;
	hsui::LabelCtrl mPWLabel;
	hsui::EditCtrl mPWEdit;

	hsui::LabelCtrl mDirLabel;
	hsui::EditCtrl mDirEdit;

	hsui::ButtonCtrl mOK;
	hsui::ButtonCtrl mCancel;
};

} // namespace hsmn
