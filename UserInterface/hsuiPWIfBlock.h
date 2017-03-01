#pragma once

#include <hsmoIfBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiComboCtrl.h>
#include <hsuiLabelCtrl.h>
#include <hsuiEditCtrl.h>
#include <hsuiRadioCtrl.h>

namespace hsui {

class PWIfBlock : public ParamWindow
{
public:
	PWIfBlock(xsc::Component* component);
	virtual ~PWIfBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;

protected:
	ComboCtrl mLeftCombo;
	EditCtrl mLeftEdit;

	LabelCtrl mOperatorLabel;
	ComboCtrl mOperatorCombo;

	ComboCtrl mRightCombo;
	EditCtrl mRightEdit;

	// ================================================================================================================
	private: friend Module;
	static void InitPWIfBlock();
	static void TermPWIfBlock();
	// ================================================================================================================
};

} // namespace hsui
