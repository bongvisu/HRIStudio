#include "hsuiPrecompiled.h"
#include "hsuiPWVariableBlock.h"
#include "hsuiVisualManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWVariableBlock(Component* component)
{
	return new PWVariableBlock(component);
}

void PWVariableBlock::InitPWVariableBlock()
{
	msFactory[XSC_RTTI(VariableBlock).GetName()] = CreatePWVariableBlock;
}

void PWVariableBlock::TermPWVariableBlock()
{
}
// ====================================================================================================================

PWVariableBlock::PWVariableBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWVariableBlock::~PWVariableBlock()
{
}

void PWVariableBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"VARIABLE - %s", mComponent->GetName().c_str());
}

void PWVariableBlock::GetDimension(int& w, int& h) const
{
	w = 400;
	h = 84;
}

void PWVariableBlock::OnInitWnd()
{
	auto block = static_cast<VariableBlock*>(mComponent);

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;
	int h = EditCtrl::FIXED_HEIGHT;
	CRect rect;
	
	mNameLabel.CreateWnd(this, L"Name", x, y, 80, h);
	mNameLabel.SetDrawBorder(true);

	HSUI_PW_LOCAL_RECT(mNameLabel, rect);

	x = rect.right + INTERCONTROL_GAP;
	mNameEdit.CreateWnd(this, x, y, 303);
	mNameEdit.SetEditText(block->GetVariable().c_str());

	x = rect.left;
	y = rect.bottom + INTERCONTROL_GAP;
	mActionLabel.CreateWnd(this, L"Action", x, y, rect.Width(), h);
	mActionLabel.SetDrawBorder(true);

	static const wchar_t* OPERATION_DISPLAY_TEXTS[NUMBER_OF_VARIABLE_OPERATIONS] =
	{
		L" Assign ( = )",
		L" Add ( + )",
		L" Subtract ( - )",
		L" Multiply ( x )",
		L" Divide ( / )"
	};

	x = rect.right + INTERCONTROL_GAP;
	mActionCombo.CreateWnd(this, x, y, 140);
	mActionCombo.SetCurrentItem(4);
	for (int i = 0; i < NUMBER_OF_VARIABLE_OPERATIONS; ++i)
	{
		ComboCtrl::Item comboItem;
		comboItem.text = OPERATION_DISPLAY_TEXTS[i];
		mActionCombo.AddItem(comboItem);
	}
	mActionCombo.SetCurrentItem(block->GetOperation());

	HSUI_PW_LOCAL_RECT(mActionCombo, rect);
	x = rect.right + INTERCONTROL_GAP;
	mActionEdit.CreateWnd(this, x, y, 160);
	mActionEdit.SetEditText(block->GetExpression().c_str());
}

void PWVariableBlock::OnApply()
{
	auto block = static_cast<VariableBlock*>(mComponent);

	block->SetVariable((const wchar_t*)mNameEdit.GetEditText());
	block->SetOperation(static_cast<VariableOperation>(mActionCombo.GetCurrentItem()));
	block->SetExpression((const wchar_t*)mActionEdit.GetEditText());
}

} // namespace hsui
