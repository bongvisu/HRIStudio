#include "hsmoPrecompiled.h"
#include "hsmoIfBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNCREATE(hsmo, IfBlock, HriBlock)

// ====================================================================================================================
void IfBlock::InitIfBlock()
{
	XSC_REGISTER_FACTORY(hsmo, IfBlock)
}

void IfBlock::TermIfBlock()
{
}
// ====================================================================================================================

IfBlock::IfBlock()
{
	mName = L"If";

	mLeftOperandType = OPERAND_TYPE_VARIABLE;
	mLeftOperand = L"VARIABLE";

	mComparisonOperator = COMPARISON_OPERATOR_EQUAL_TO;
	
	mRightOperandType = OPERAND_TYPE_VALUE;
	mRightOperand = L"VALUE";
}

IfBlock::~IfBlock()
{
}

void IfBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 50;
	mGeometry.t = center.y - 50;
	mGeometry.r = mGeometry.l + 101;
	mGeometry.b = mGeometry.t + 101;

	AddInputPort(MPRectangle::EDGE_L);
	AddOutputPort(MPRectangle::EDGE_R);
	AddOutputPort(MPRectangle::EDGE_R);
}

void IfBlock::SetLeftOperandType(OperandType operandType)
{
	mLeftOperandType = operandType;
}

OperandType IfBlock::GetLeftOperandType() const
{
	return mLeftOperandType;
}

void IfBlock::SetRightOperandType(OperandType operandType)
{
	mRightOperandType = operandType;
}

OperandType IfBlock::GetRightOperandType() const
{
	return mRightOperandType;
}

void IfBlock::SetLeftOperand(const wstring& variable)
{
	mLeftOperand = variable;
}

const wstring& IfBlock::GetLeftOperand() const
{
	return mLeftOperand;
}

void IfBlock::SetComparisonOperator(ComparisonOperator comparisonOperator)
{
	mComparisonOperator = comparisonOperator;
}

ComparisonOperator IfBlock::GetComparisonOperator() const
{
	return mComparisonOperator;
}

void IfBlock::SetRightOperand(const wstring& value)
{
	mRightOperand = value;
}

const wstring& IfBlock::GetRightOperand() const
{
	return mRightOperand;
}

// Code Generation ----------------------------------------------------------------------------------------------------
wstring IfBlock::GetEntryCode() const
{
	swprintf_s(msGenericTextBuf, L"{topic=IF_%llu}{@init}", mID);
	return msGenericTextBuf;
}

bool IfBlock::GenerateCode(wstring& code, int level)
{
	code += L'\n';

	// topic start
	swprintf_s(msGenericTextBuf, L"> topic IF_%llu", mID);
	code += msGenericTextBuf;
	code += L"\n";

	// init line
	code += L"\t+ init";
	code += L"\n";

	// condition line
	code += L"\t";
	code += L"* ";
	if (mLeftOperandType == OPERAND_TYPE_VARIABLE)
	{
		swprintf_s(msGenericTextBuf, L"<get %s>", mLeftOperand.c_str());
	}
	else
	{
		swprintf_s(msGenericTextBuf, L"%s", mLeftOperand.c_str());
	}
	code += msGenericTextBuf;

	code += L" ";
	code += COMPARISON_OPERATOR_CODES[mComparisonOperator];
	code += L" ";

	if (mRightOperandType == OPERAND_TYPE_VARIABLE)
	{
		swprintf_s(msGenericTextBuf, L"<get %s>", mRightOperand.c_str());
	}
	else
	{
		swprintf_s(msGenericTextBuf, L"%s", mRightOperand.c_str());
	}
	code += msGenericTextBuf;

	code += L" =>";

	// true
	{
		const auto& joints = mOutputPorts[0]->GetConnectionJoints();
		if (joints.size())
		{
			for (auto joint : joints)
			{
				auto link = static_cast<Link*>(joint->GetParent());
				auto destPort = link->GetDestinationPort();
				auto destBlock = static_cast<const HriBlock*>(destPort->GetParent());

				code += L" ";
				code += destBlock->GetEntryCode();
			}
		}
		else		
		{
			code += L" {topic=random}";
		}
	}

	code += L"\n";
	
	// fallback(false) line
	{
		code += L"\t-";

		const auto& joints = mOutputPorts[1]->GetConnectionJoints();
		if (joints.size())
		{
			for (auto joint : joints)
			{
				auto link = static_cast<Link*>(joint->GetParent());
				auto destPort = link->GetDestinationPort();
				auto destBlock = static_cast<const HriBlock*>(destPort->GetParent());

				code += L" ";
				code += destBlock->GetEntryCode();
			}
		}
		else
		{
			code += L" {topic=random}";
		}
	}
	
	// topic end
	code += L"\n";
	code += L"< topic";

	code += L"\n";
	return true;
}
// --------------------------------------------------------------------------------------------------------------------

// Cloning ------------------------------------------------------------------------------------------------------------
Component* IfBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<IfBlock*>(HriBlock::Clone(remapper));

	cloned->mLeftOperandType = mLeftOperandType;
	cloned->mLeftOperand = mLeftOperand;

	cloned->mComparisonOperator = mComparisonOperator;

	cloned->mRightOperandType = mRightOperandType;
	cloned->mRightOperand = mRightOperand;

	return cloned;
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool IfBlock::Serialize(Stream& stream)
{
	if (false == HriBlock::Serialize(stream))
	{
		return false;
	}

	stream.BeginOutputSection(L"LeftOperandType", true, true);
	swprintf_s(msGenericTextBuf, L"%u", mLeftOperandType);
	stream.SetSimpleValaue(msGenericTextBuf);
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"LeftOperand", true, true);
	stream.SetCDataValue(mLeftOperand.c_str());
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"ComparisonOperator", true, true);
	swprintf_s(msGenericTextBuf, L"%u", mComparisonOperator);
	stream.SetSimpleValaue(msGenericTextBuf);
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"RightOperandType", true, true);
	swprintf_s(msGenericTextBuf, L"%u", mRightOperandType);
	stream.SetSimpleValaue(msGenericTextBuf);
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"RightOperand", true, true);
	stream.SetCDataValue(mRightOperand.c_str());
	stream.EndOutputSection(false, false);

	return true;
}

bool IfBlock::Deserialize(Stream& stream)
{
	if (false == HriBlock::Deserialize(stream))
	{
		return false;
	}

	wstring value;
	int numElements;
	bool failed = false;

	numElements = stream.BeginInputSection(L"LeftOperandType");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(value);
		mLeftOperandType = static_cast<OperandType>(std::stoul(value));
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	numElements = stream.BeginInputSection(L"LeftOperand");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(mLeftOperand);
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	numElements = stream.BeginInputSection(L"ComparisonOperator");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(value);
		mComparisonOperator = static_cast<ComparisonOperator>(std::stoul(value));
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	numElements = stream.BeginInputSection(L"RightOperandType");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(value);
		mRightOperandType = static_cast<OperandType>(std::stoul(value));
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	numElements = stream.BeginInputSection(L"RightOperand");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(mRightOperand);
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
