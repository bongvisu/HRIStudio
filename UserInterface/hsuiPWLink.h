#pragma once

#include <xscLink.h>
#include <hsuiParamWindow.h>
#include <hsuiComboCtrl.h>
#include <hsuiLabelCtrl.h>

namespace hsui {

class PWLink : public ParamWindow
{
public:
	PWLink(xsc::Component* component);
	virtual ~PWLink();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;

protected:
	LabelCtrl mWidthLabel;
	ComboCtrl mWidthCombo;

	LabelCtrl mStyleLabel;
	ComboCtrl mStyleCombo;

	LabelCtrl mColorLabel;
	ComboCtrl mColorCombo;

	// ================================================================================================================
	private: friend Module;
	static void InitPWLink();
	static void TermPWLink();
	// ================================================================================================================
};

} // namespace hsui
