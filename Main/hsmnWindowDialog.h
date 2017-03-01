#pragma once

#include <hsuiDialog.h>
#include <hsuiButtonCtrl.h>
#include <hsuiListCtrl.h>

namespace hsmn {

class WindowDialog
	:
	public hsui::Dialog,
	public hsui::ButtonCtrl::IListener
{
public:
	WindowDialog();
	virtual ~WindowDialog();

protected:
	virtual void OnOK() override;
	virtual void InitWindow() override;
	virtual void OnClicked(hsui::ButtonCtrl* buttonCtrl) override;
	virtual void PaintContent(CDC* dc) override;

protected:
	hsui::ListCtrl mList;
	hsui::ButtonCtrl mActivateButton;
};

} // namespace hsmn
