#pragma once

#include <hsmoSwitchBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiEditCtrl.h>
#include <hsuiLabelCtrl.h>
#include <hsuiRadioCtrl.h>
#include <hsuiComboCtrl.h>

namespace hsui {

class PWSwitchBlock : public ParamWindow
{
public:
	PWSwitchBlock(xsc::Component* component);
	virtual ~PWSwitchBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;
	virtual void OnButtonClick(ButtonCtrl* buttonCtrl) override;
	virtual void OnPaintWnd();

protected:
	ButtonCtrl  mPlus;
	ButtonCtrl  mMinus;

	LabelCtrl	mCase[hsmo::SwitchBlock::MAX_NUM_CASES];
	ComboCtrl	mLeftCombo[hsmo::SwitchBlock::MAX_NUM_CASES];
	EditCtrl	mLeftEdit[hsmo::SwitchBlock::MAX_NUM_CASES];
	LabelCtrl	mOperatorLabel[hsmo::SwitchBlock::MAX_NUM_CASES];
	ComboCtrl	mOperatorCombo[hsmo::SwitchBlock::MAX_NUM_CASES];
	ComboCtrl	mRightCombo[hsmo::SwitchBlock::MAX_NUM_CASES];
	EditCtrl	mRightEdit[hsmo::SwitchBlock::MAX_NUM_CASES];

	// ================================================================================================================
	private: friend Module;
	static void InitPWSwitchBlock();
	static void TermPWSwitchBlock();
	// ================================================================================================================
};

} // namespace hsui
