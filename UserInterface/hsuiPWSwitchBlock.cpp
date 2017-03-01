#include "hsuiPrecompiled.h"
#include "hsuiPWSwitchBlock.h"
#include "hsuiVisualManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWSwitchBlock(Component* component)
{
	return new PWSwitchBlock(component);
}

void PWSwitchBlock::InitPWSwitchBlock()
{
	msFactory[XSC_RTTI(SwitchBlock).GetName()] = CreatePWSwitchBlock;
}

void PWSwitchBlock::TermPWSwitchBlock()
{
}
// ====================================================================================================================

PWSwitchBlock::PWSwitchBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWSwitchBlock::~PWSwitchBlock()
{
}

void PWSwitchBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"SWITCH - %s", mComponent->GetName().c_str());
}

void PWSwitchBlock::GetDimension(int& w, int& h) const
{
	w = 500;
	h = 338;
}

void PWSwitchBlock::OnInitWnd()
{
	auto block = static_cast<SwitchBlock*>(mComponent);

	static const int OPERAND_WIDTH = 163;
	static const int OPERATOR_WIDTH = 69;
	static const int ADD_REMOVE_BUTTON_WIDTH = 241;
	static const int CASE_LABEL_WIDTH = 68;

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;
	int w = ADD_REMOVE_BUTTON_WIDTH;
	
	mPlus.CreateWnd(this, L"+", x, y, w, EditCtrl::FIXED_HEIGHT);
	mPlus.SetMode(ButtonCtrl::MODE_TEXT);
	x += ADD_REMOVE_BUTTON_WIDTH + INTERCONTROL_GAP + 1;
	mMinus.CreateWnd(this, L"-", x, y, w, EditCtrl::FIXED_HEIGHT);
	mMinus.SetMode(ButtonCtrl::MODE_TEXT);

	mPlus.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	mMinus.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	x = CONTENT_MARGIN;
	y += CONTENT_MARGIN;
	w = OPERAND_WIDTH;
	CRect rect;
	ComboCtrl::Item comboItem;
	
	for (unsigned int i = 0; i < SwitchBlock::MAX_NUM_CASES; i++)
	{
		x = 2 * CONTENT_MARGIN;
		y += EditCtrl::FIXED_HEIGHT + CONTENT_MARGIN - 1;

		swprintf_s(msTextBuf, L"Case %u", i + 1);
		mCase[i].CreateWnd(
			this, msTextBuf, x, y, CASE_LABEL_WIDTH, 2 * EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP);
		mCase[i].SetDrawBorder(true);
		mCase[i].ShowWindow(SW_HIDE);

		x += CASE_LABEL_WIDTH + INTERCONTROL_GAP;
		mLeftCombo[i].CreateWnd(this, x, y, OPERAND_WIDTH);
		mLeftCombo[i].ShowWindow(SW_HIDE);
		comboItem.image = ComboCtrl::DEFAULT_ICON;
		comboItem.text = L" Variable";
		mLeftCombo[i].AddItem(comboItem);
		comboItem.text = L" Value";
		mLeftCombo[i].AddItem(comboItem);
		comboItem.text = L" Other";
		mLeftCombo[i].AddItem(comboItem);
		mLeftCombo[i].SetCurrentItem(OPERAND_TYPE_VARIABLE);

		y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
		mLeftEdit[i].CreateWnd(this, x, y, OPERAND_WIDTH);
		mLeftEdit[i].GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
		mLeftEdit[i].SetEditText(L"VARIABLE");
		mLeftEdit[i].ShowWindow(SW_HIDE);

		HSUI_PW_LOCAL_RECT(mLeftCombo[i], rect);
		x = rect.right + INTERCONTROL_GAP;
		y = rect.top;
		mOperatorLabel[i].CreateWnd(this, L"Operator", x, y, OPERATOR_WIDTH, EditCtrl::FIXED_HEIGHT);
		mOperatorLabel[i].SetDrawBorder(true);
		mOperatorLabel[i].ShowWindow(SW_HIDE);

		y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
		mOperatorCombo[i].CreateWnd(this, x, y, OPERATOR_WIDTH);
		comboItem.image = ComboCtrl::NO_ITEM_ICON;
		for (int j = 0; j < NUMBER_OF_COMPARISON_OPERATORS; j++)
		{
			comboItem.text = L"    ";
			comboItem.text += COMPARISON_OPERATOR_CODES[j];
			mOperatorCombo[i].AddItem(comboItem);
		}
		mOperatorCombo[i].SetCurrentItem(0);
		mOperatorCombo[i].ShowWindow(SW_HIDE);

		HSUI_PW_LOCAL_RECT(mOperatorLabel[i], rect);
		ScreenToClient(&rect);
		x = rect.right + INTERCONTROL_GAP;
		y = rect.top;
		mRightCombo[i].CreateWnd(this, x, y, OPERAND_WIDTH);
		mRightCombo[i].ShowWindow(SW_HIDE);
		comboItem.image = ComboCtrl::DEFAULT_ICON;
		comboItem.text = L" Variable";
		mRightCombo[i].AddItem(comboItem);
		comboItem.text = L" Value";
		mRightCombo[i].AddItem(comboItem);
		comboItem.text = L" Other";
		mRightCombo[i].AddItem(comboItem);
		mRightCombo[i].SetCurrentItem(OPERAND_TYPE_VALUE);

		y += EditCtrl::FIXED_HEIGHT + INTERCONTROL_GAP;
		mRightEdit[i].CreateWnd(this, x, y, OPERAND_WIDTH);
		mRightEdit[i].GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
		mRightEdit[i].SetEditText(L"VALUE");
		mRightEdit[i].ShowWindow(SW_HIDE);
	}

	auto& cases = block->GetCases();
	unsigned int numCases = static_cast<unsigned int>(cases.size());
	for (unsigned int i = 0; i < numCases; i++)
	{
		mCase[i].ShowWindow(SW_SHOW);

		mLeftCombo[i].SetCurrentItem(cases[i].leftOperandType);
		mLeftCombo[i].ShowWindow(SW_SHOW);

		mLeftEdit[i].SetEditText(cases[i].leftOperand.c_str());
		mLeftEdit[i].ShowWindow(SW_SHOW);

		mOperatorLabel[i].ShowWindow(SW_SHOW);
		mOperatorCombo[i].SetCurrentItem(cases[i].comparisonOperator);
		mOperatorCombo[i].ShowWindow(SW_SHOW);

		mRightCombo[i].SetCurrentItem(cases[i].rightOperandType);
		mRightCombo[i].ShowWindow(SW_SHOW);

		mRightEdit[i].SetEditText(cases[i].rightOperand.c_str());
		mRightEdit[i].ShowWindow(SW_SHOW);
	}
}

void PWSwitchBlock::OnButtonClick(ButtonCtrl* buttonCtrl)
{
	if (&mPlus == buttonCtrl)
	{
		for (unsigned int i = SwitchBlock::MIN_NUM_CASES; i < SwitchBlock::MAX_NUM_CASES; i++)
		{
			if (FALSE == mCase[i].IsWindowVisible())
			{
				mCase[i].ShowWindow(TRUE);
				mLeftCombo[i].ShowWindow(TRUE);
				mLeftEdit[i].ShowWindow(TRUE);
				mOperatorLabel[i].ShowWindow(TRUE);
				mOperatorCombo[i].ShowWindow(TRUE);
				mRightCombo[i].ShowWindow(TRUE);
				mRightEdit[i].ShowWindow(TRUE);
				break;
			}
		}
	}
	else if (&mMinus == buttonCtrl)
	{
		for (unsigned int i = (SwitchBlock::MAX_NUM_CASES - 1); i >= SwitchBlock::MIN_NUM_CASES; i--)
		{
			if (mCase[i].IsWindowVisible())
			{
				mCase[i].ShowWindow(FALSE);
				mLeftCombo[i].ShowWindow(FALSE);
				mLeftEdit[i].ShowWindow(FALSE);
				mOperatorLabel[i].ShowWindow(FALSE);
				mOperatorCombo[i].ShowWindow(FALSE);
				mRightCombo[i].ShowWindow(FALSE);
				mRightEdit[i].ShowWindow(FALSE);
				break;
			}
		}
	}
}

void PWSwitchBlock::OnApply()
{
	auto block = static_cast<SwitchBlock*>(mComponent);

	wstring text;
	CString operand;

	CaseDescriptionList cases;
	cases.reserve(SwitchBlock::MAX_NUM_CASES);

	for (unsigned int i = 0; i < SwitchBlock::MAX_NUM_CASES; i++)
	{
		if (mCase[i].IsWindowVisible())
		{
			CaseDescription caseDesc;

			caseDesc.leftOperandType = static_cast<OperandType>(mLeftCombo[i].GetCurrentItem());
			operand = mLeftEdit[i].GetEditText();
			operand.Trim();
			caseDesc.leftOperand = (const wchar_t*)operand;

			caseDesc.comparisonOperator = static_cast<ComparisonOperator>(mOperatorCombo[i].GetCurrentItem());

			caseDesc.rightOperandType = static_cast<OperandType>(mRightCombo[i].GetCurrentItem());
			operand = mRightEdit[i].GetEditText();
			operand.Trim();
			caseDesc.rightOperand = (const wchar_t*)operand;

			cases.push_back(caseDesc);
		}
		else
		{
			break;
		}
	}

	block->SetCases(std::move(cases));
}

void PWSwitchBlock::OnPaintWnd()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	mBrush->SetColor(VisualManager::CLR_LEVEL3);

	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(CONTENT_MARGIN + 1);
	rect.top = static_cast<FLOAT>(TOP_MARGIN + EditCtrl::FIXED_HEIGHT + CONTENT_MARGIN);
	rect.right = static_cast<FLOAT>(clientRect.right - CONTENT_MARGIN);
	rect.bottom = static_cast<FLOAT>(clientRect.bottom - CONTENT_MARGIN);
	mRenderer.DrawRectangle(rect, mBrush);
}

} // namespace hsui
