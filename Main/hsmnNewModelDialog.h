#pragma once

#include <hsuiDialog.h>
#include <hsuiButtonCtrl.h>
#include <hsuiLabelCtrl.h>
#include <hsuiEditCtrl.h>

namespace hsmn {

class NewModelDialog
	:
	public hsui::Dialog,
	public hsui::ButtonCtrl::IListener
{
public:
	NewModelDialog();
	virtual ~NewModelDialog();

protected:
	virtual void OnOK() override;
	virtual void InitWindow() override;
	virtual void OnClicked(hsui::ButtonCtrl* buttonCtrl) override;

protected:
	hsui::LabelCtrl mLocationLabel;
	hsui::EditCtrl mLocationEdit;
	hsui::ButtonCtrl mLocationButton;

	hsui::LabelCtrl mNameLabel;
	hsui::EditCtrl mNameEdit;
	hsui::ButtonCtrl mOKButton;
};

} // namespace hsmn
