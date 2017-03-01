#include "hsmoPrecompiled.h"
#include "hsmoVariableBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNCREATE(hsmo, VariableBlock, HriBlock)

// ====================================================================================================================
void VariableBlock::InitVariableBlock()
{
	XSC_REGISTER_FACTORY(hsmo, VariableBlock)
}

void VariableBlock::TermVariableBlock()
{
}
// ====================================================================================================================

VariableBlock::VariableBlock()
{
	mName = L"Var";

	mVariable = L"UserVariable";
	mOperation = VARIABLE_OPERATION_ASSIGN;
	mExpression = L"0";
}

VariableBlock::~VariableBlock()
{
}

void VariableBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddInputPort(MPRectangle::EDGE_L);
	AddOutputPort(MPRectangle::EDGE_R);
}

void VariableBlock::SetVariable(const wstring& variable)
{
	mVariable = variable;
}

const wstring& VariableBlock::GetVariable() const
{
	return mVariable;
}

void VariableBlock::SetOperation(VariableOperation operation)
{
	mOperation = operation;
}

VariableOperation VariableBlock::GetOperation() const
{
	return mOperation;
}

void VariableBlock::SetExpression(const wstring& expression)
{
	mExpression = expression;
}

const wstring& VariableBlock::GetExpression() const
{
	return mExpression;
}

// Code Generation ----------------------------------------------------------------------------------------------------
wstring VariableBlock::GetEntryCode() const
{
	swprintf_s(msGenericTextBuf, L"{topic=VAR_%llu}{@init}", mID);
	return msGenericTextBuf;
}

bool VariableBlock::GenerateCode(wstring& code, int level)
{
	code += L'\n';

	// topic start
	swprintf_s(msGenericTextBuf, L"> topic VAR_%llu", mID);
	code += msGenericTextBuf;
	code += L"\n";

	// init line
	code += L"\t+ init";
	code += L"\n";

	// response line
	code += L"\t- <";
	switch (mOperation)
	{
	case VARIABLE_OPERATION_ASSIGN:
		code += L"set ";
		break;
	case VARIABLE_OPERATION_ADDITION:
		code += L"add ";
		break;
	case VARIABLE_OPERATION_SUBTRACTION:
		code += L"sub ";
		break;
	case VARIABLE_OPERATION_MULTIPLICATION:
		code += L"mult ";
		break;
	case VARIABLE_OPERATION_DIVISION:
		code += L"div ";
		break;
	}
	code += mVariable;
	code += L"=";
	code += mExpression;
	code += L">";
	
	// transition
	code += L"\n\t^";
	auto& joints = mOutputPorts[0]->GetConnectionJoints();
	for (auto joint : joints)
	{
		auto parent = joint->GetParent();
		assert(parent->IsDerivedFrom(XSC_RTTI(Link)));
		auto link = static_cast<Link*>(parent);
		auto dest = link->GetDestinationPort();
		assert(dest);

		auto destBlock = static_cast<const HriBlock*>(dest->GetParent());
		code += L" ";
		code += destBlock->GetEntryCode();
	}
	if (joints.size() == 0)
	{
		code += L" {topic=random}";
	}

	// topic end
	code += L"\n";
	code += L"< topic";

	code += L"\n";
	return true;
}
// --------------------------------------------------------------------------------------------------------------------

// Cloning ------------------------------------------------------------------------------------------------------------
Component* VariableBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<VariableBlock*>(HriBlock::Clone(remapper));

	cloned->mVariable = mVariable;
	cloned->mOperation = mOperation;
	cloned->mExpression = mExpression;

	return cloned;
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool VariableBlock::Serialize(Stream& stream)
{
	if (false == HriBlock::Serialize(stream))
	{
		return false;
	}

	stream.BeginOutputSection(L"Variable", true, true);
	stream.SetCDataValue(mVariable.c_str());
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"Operation", true, true);
	swprintf_s(msGenericTextBuf, L"%u", mOperation);
	stream.SetSimpleValaue(msGenericTextBuf);
	stream.EndOutputSection(false, false);

	stream.BeginOutputSection(L"Expression", true, true);
	stream.SetCDataValue(mExpression.c_str());
	stream.EndOutputSection(false, false);

	return true;
}

bool VariableBlock::Deserialize(Stream& stream)
{
	if (false == HriBlock::Deserialize(stream))
	{
		return false;
	}

	wstring value;
	int numElements;

	numElements = stream.BeginInputSection(L"Variable");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(mVariable);
		stream.DeactivateItem();
	}
	stream.EndInputSection();

	numElements = stream.BeginInputSection(L"Operation");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(value);
		mOperation = static_cast<VariableOperation>(std::stoul(value));
		stream.DeactivateItem();
	}
	stream.EndInputSection();

	numElements = stream.BeginInputSection(L"Expression");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(mExpression);
		stream.DeactivateItem();
	}
	stream.EndInputSection();

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
