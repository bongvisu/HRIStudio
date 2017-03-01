#pragma once

#include <hsmoVariableBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiEditCtrl.h>
#include <hsuiLabelCtrl.h>
#include <hsuiComboCtrl.h>

namespace hsui {

class PWVariableBlock : public ParamWindow
{
public:
	PWVariableBlock(xsc::Component* component);
	virtual ~PWVariableBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;

protected:
	LabelCtrl mNameLabel;
	EditCtrl mNameEdit;

	LabelCtrl mActionLabel;
	ComboCtrl mActionCombo;
	EditCtrl mActionEdit;

	// ================================================================================================================
	private: friend Module;
	static void InitPWVariableBlock();
	static void TermPWVariableBlock();
	// ================================================================================================================
};

} // namespace hsui
