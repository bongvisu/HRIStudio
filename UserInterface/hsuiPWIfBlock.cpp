#include "hsuiPrecompiled.h"
#include "hsuiPWIfBlock.h"
#include "hsuiVisualManager.h"
#include "hsuiEditCtrl.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWIfBlock(Component* component)
{
	return new PWIfBlock(component);
}

void PWIfBlock::InitPWIfBlock()
{
	msFactory[XSC_RTTI(IfBlock).GetName()] = CreatePWIfBlock;
}

void PWIfBlock::TermPWIfBlock()
{
}
// ====================================================================================================================

PWIfBlock::PWIfBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWIfBlock::~PWIfBlock()
{
}

void PWIfBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"IF - %s", mComponent->GetName().c_str());
}

void PWIfBlock::GetDimension(int& w, int& h) const
{
	w = 400;
	h = 84;
}

void PWIfBlock::OnInitWnd()
{
	auto block = static_cast<IfBlock*>(mComponent);

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;
	CRect rect;

	static const int OPERAND_WIDTH = 157;
	static const int OPERATOR_WIDTH = 66;
	int w = OPERAND_WIDTH;
	ComboCtrl::Item comboItem;

	mLeftCombo.CreateWnd(this, x, y, w);
	comboItem.text = L" Variable";
	mLeftCombo.AddItem(comboItem);
	comboItem.text = L" Value";
	mLeftCombo.AddItem(comboItem);
	comboItem.text = L" Other";
	mLeftCombo.AddItem(comboItem);
	mLeftCombo.SetCurrentItem(block->GetLeftOperandType());

	HSUI_PW_LOCAL_RECT(mLeftCombo, rect);
	x = rect.left;
	y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
	w = rect.Width();
	mLeftEdit.CreateWnd(this, x, y, w);
	mLeftEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
	mLeftEdit.SetEditText(block->GetLeftOperand().c_str());

	x = rect.right + INTERCONTROL_GAP;
	y = rect.top;
	w = OPERATOR_WIDTH;
	mOperatorLabel.CreateWnd(this, L"Operator", x, y, w, EditCtrl::FIXED_HEIGHT);
	mOperatorLabel.SetDrawBorder(true);
	
	y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
	mOperatorCombo.CreateWnd(this, x, y, w);
	comboItem.image = ComboCtrl::NO_ITEM_ICON;
	for (int i = 0; i < NUMBER_OF_COMPARISON_OPERATORS; i++)
	{
		comboItem.text = L"    ";
		comboItem.text += COMPARISON_OPERATOR_CODES[i];
		mOperatorCombo.AddItem(comboItem);
	}
	mOperatorCombo.SetCurrentItem(block->GetComparisonOperator());

	HSUI_PW_LOCAL_RECT(mOperatorLabel, rect);
	x = rect.right + INTERCONTROL_GAP;
	y = rect.top;
	w = OPERAND_WIDTH;
	comboItem.image = ComboCtrl::DEFAULT_ICON;
	mRightCombo.CreateWnd(this, x, y, w);
	comboItem.text = L" Variable";
	mRightCombo.AddItem(comboItem);
	comboItem.text = L" Value";
	mRightCombo.AddItem(comboItem);
	comboItem.text = L" Other";
	mRightCombo.AddItem(comboItem);
	mRightCombo.SetCurrentItem(block->GetRightOperandType());

	y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
	mRightEdit.CreateWnd(this, x, y, w);
	mRightEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
	mRightEdit.SetEditText(block->GetRightOperand().c_str());
}

void PWIfBlock::OnApply()
{
	auto block = static_cast<IfBlock*>(mComponent);

	wstring text;
	CString operand;
	
	block->SetLeftOperandType(static_cast<OperandType>(mLeftCombo.GetCurrentItem()));
	operand = mLeftEdit.GetEditText();
	operand.Trim();
	text = ((const wchar_t*)operand);
	block->SetLeftOperand(text);

	block->SetComparisonOperator(static_cast<ComparisonOperator>(mOperatorCombo.GetCurrentItem()));

	block->SetRightOperandType(static_cast<OperandType>(mRightCombo.GetCurrentItem()));
	operand = mRightEdit.GetEditText();
	operand.Trim();
	text = (const wchar_t*)operand;
	block->SetRightOperand(text);
}

} // namespace hsui
