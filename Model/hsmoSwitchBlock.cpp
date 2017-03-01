#include "hsmoPrecompiled.h"
#include "hsmoSwitchBlock.h"
#include <xscLevel.h>

using namespace std;
using namespace xsc;

namespace hsmo {

CaseDescription::CaseDescription()
{
	leftOperandType = OPERAND_TYPE_VARIABLE;
	leftOperand = L"VARIABLE";

	comparisonOperator = COMPARISON_OPERATOR_EQUAL_TO;

	rightOperandType = OPERAND_TYPE_VALUE;
	rightOperand = L"VALUE";
}

CaseDescription::CaseDescription(CaseDescription&& caseDescription)
{
	this->operator=(std::move(caseDescription));
}

CaseDescription& CaseDescription::operator=(CaseDescription&& caseDescription)
{
	leftOperandType = caseDescription.leftOperandType;
	leftOperand = std::move(caseDescription.leftOperand);

	comparisonOperator = caseDescription.comparisonOperator;

	rightOperandType = caseDescription.rightOperandType;
	rightOperand = std::move(caseDescription.rightOperand);

	return *this;
}

CaseDescription::CaseDescription(const CaseDescription& caseDescription)
{
	this->operator=(caseDescription);
}

CaseDescription& CaseDescription::operator=(const CaseDescription& caseDescription)
{
	leftOperandType = caseDescription.leftOperandType;
	leftOperand = caseDescription.leftOperand;

	comparisonOperator = caseDescription.comparisonOperator;

	rightOperandType = caseDescription.rightOperandType;
	rightOperand = caseDescription.rightOperand;

	return *this;
}

// ====================================================================================================================
void SwitchBlock::InitSwitchBlock()
{
	XSC_REGISTER_FACTORY(hsmo, SwitchBlock)
}

void SwitchBlock::TermSwitchBlock()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, SwitchBlock, HriBlock)

SwitchBlock::SwitchBlock()
{
	mName = L"Switch";
}

SwitchBlock::~SwitchBlock()
{
}

void SwitchBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 50;
	mGeometry.t = center.y - 50;
	mGeometry.r = mGeometry.l + 101;
	mGeometry.b = mGeometry.t + 101;

	AddInputPort(MPRectangle::EDGE_L);
	
	mCases.resize(2);
	AddOutputPort(MPRectangle::EDGE_R);
	AddOutputPort(MPRectangle::EDGE_R);

	AddOutputPort(MPRectangle::EDGE_R);
}

void SwitchBlock::SetCases(CaseDescriptionList&& cases)
{
	int orgNumCases = static_cast<int>(mCases.size());
	int newNumCases = static_cast<int>(cases.size());
	assert(static_cast<int>(mOutputPorts.size() - 1) == orgNumCases);
	
	mCases = std::move(cases);

	auto level = dynamic_cast<ILevel*>(GetLevel());
	if (orgNumCases > newNumCases)
	{
		int diff = orgNumCases - newNumCases;
		
		PortSet ports;
		ports.reserve(diff);

		for (int i = newNumCases; i < orgNumCases; i++)
		{
			ports.insert(mOutputPorts[i]);
		}
		level->HandlePortRemoval(ports);

		NotifyChildRemoval(reinterpret_cast<const ComponentSet&>(ports));
		RemovePorts(ports);

		UpdatePortGeometry();
		UpdateConnectedLinks();

		NotifyNodalGeometryChange();
		level->OnChildGeometryChange();
	}
	else if (orgNumCases < newNumCases)
	{
		int diff = newNumCases - orgNumCases;
		auto attachSide = mOutputPorts[0]->GetAttachSide();

		PortSet ports;
		auto defaultPort = *mOutputPorts.rbegin();
		ports.insert(defaultPort);
		RemovePorts(ports, false);

		ComponentSet newComponents;
		for (int i = 0; i < diff; i++)
		{
			newComponents.insert(AddOutputPort(attachSide));
		}
		mOutputPorts.push_back(defaultPort);
		mPorts[attachSide].push_back(defaultPort);

		UpdatePortGeometry();
		UpdateConnectedLinks();

		NotifyChildAddition(newComponents);

		NotifyNodalGeometryChange();
		level->OnChildGeometryChange();
	}
}

const CaseDescriptionList& SwitchBlock::GetCases() const
{
	return mCases;
}

// Code Generation ----------------------------------------------------------------------------------------------------
wstring SwitchBlock::GetEntryCode() const
{
	swprintf_s(msGenericTextBuf, L"{topic=SWITCH_%llu}{@init}", mID);
	return msGenericTextBuf;
}

bool SwitchBlock::GenerateCode(wstring& code, int level)
{
	code += L'\n';

	// topic start
	swprintf_s(msGenericTextBuf, L"> topic SWITCH_%llu", mID);
	code += msGenericTextBuf;
	code += L"\n";

	// init line
	code += L"\t+ init";

	// case lines
	unsigned int numCases = static_cast<unsigned int>(mCases.size());
	assert(numCases >= MIN_NUM_CASES);
	assert((numCases + 1) == static_cast<unsigned int>(mOutputPorts.size()));
	for (unsigned int i = 0; i < numCases; i++)
	{
		code += L"\n";
		code += L"\t* ";

		if (mCases[i].leftOperandType == OPERAND_TYPE_VARIABLE)
		{
			swprintf_s(msGenericTextBuf, L"<get %s>", mCases[i].leftOperand.c_str());
		}
		else
		{
			swprintf_s(msGenericTextBuf, L"%s", mCases[i].leftOperand.c_str());
		}
		code += msGenericTextBuf;

		code += L" ";
		code += COMPARISON_OPERATOR_CODES[mCases[i].comparisonOperator];
		code += L" ";

		if (mCases[i].rightOperandType == OPERAND_TYPE_VARIABLE)
		{
			swprintf_s(msGenericTextBuf, L"<get %s>", mCases[i].rightOperand.c_str());
		}
		else
		{
			swprintf_s(msGenericTextBuf, L"%s", mCases[i].rightOperand.c_str());
		}
		code += msGenericTextBuf;

		code += L" =>";

		const auto& joints = mOutputPorts[i]->GetConnectionJoints();
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
	}

	// fallback(default) line
	code += L"\n";
	code += L"\t-";
	const auto& joints = mOutputPorts[numCases]->GetConnectionJoints();
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

// Memento ------------------------------------------------------------------------------------------------------------
MementoPtr SwitchBlock::SaveState() const
{
	return new SwitchBlockMemento(const_cast<SwitchBlock*>(this));
}

SwitchBlock::SwitchBlockMemento::SwitchBlockMemento(SwitchBlock* block)
	:
	BlockMemento(block)
{
	mCases = block->mCases;
}

void SwitchBlock::SwitchBlockMemento::Restore()
{
	auto block = static_cast<SwitchBlock*>(mComponent);

	block->mCases = mCases;

	BlockMemento::Restore();
}
// --------------------------------------------------------------------------------------------------------------------

// Cloning ------------------------------------------------------------------------------------------------------------
Component* SwitchBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<SwitchBlock*>(HriBlock::Clone(remapper));

	cloned->mCases = mCases;

	return cloned;
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool SwitchBlock::Serialize(Stream& stream)
{
	if (false == HriBlock::Serialize(stream))
	{
		return false;
	}

	for (const auto& caseDesc : mCases)
	{
		stream.BeginOutputSection(L"Case", true, true);
		{
			stream.BeginOutputSection(L"LeftOperandType", true, true);
			swprintf_s(msGenericTextBuf, L"%u", caseDesc.leftOperandType);
			stream.SetSimpleValaue(msGenericTextBuf);
			stream.EndOutputSection(false, false);

			stream.BeginOutputSection(L"LeftOperand", true, true);
			stream.SetCDataValue(caseDesc.leftOperand.c_str());
			stream.EndOutputSection(false, false);
		
			stream.BeginOutputSection(L"ComparisonOperator", true, true);
			swprintf_s(msGenericTextBuf, L"%u", caseDesc.comparisonOperator);
			stream.SetSimpleValaue(msGenericTextBuf);
			stream.EndOutputSection(false, false);

			stream.BeginOutputSection(L"RightOperandType", true, true);
			swprintf_s(msGenericTextBuf, L"%u", caseDesc.rightOperandType);
			stream.SetSimpleValaue(msGenericTextBuf);
			stream.EndOutputSection(false, false);

			stream.BeginOutputSection(L"RightOperand", true, true);
			stream.SetCDataValue(caseDesc.rightOperand.c_str());
			stream.EndOutputSection(false, false);
		}
		stream.EndOutputSection(true, true);
	}

	return true;
}

bool SwitchBlock::Deserialize(Stream& stream)
{
	if (false == HriBlock::Deserialize(stream))
	{
		return false;
	}

	wstring value;
	int numCases;
	int numElements;
	bool failed = false;

	numCases = stream.BeginInputSection(L"Case");
	if (numCases > 1)
	{
		for (int i = 0; i < numCases; i++)
		{
			CaseDescription caseDesc;

			stream.ActivateItem(i);
			{
				numElements = stream.BeginInputSection(L"LeftOperandType");
				if (numElements)
				{
					stream.ActivateItem(0);
					stream.GetTextValue(value);
					caseDesc.leftOperandType = static_cast<OperandType>(std::stoul(value));
					stream.DeactivateItem();
				}
				else
				{
					failed = true;
				}
				stream.EndInputSection();
				if (failed)
				{
					stream.DeactivateItem();
					break;
				}

				numElements = stream.BeginInputSection(L"LeftOperand");
				if (numElements)
				{
					stream.ActivateItem(0);
					stream.GetTextValue(caseDesc.leftOperand);
					stream.DeactivateItem();
				}
				else
				{
					failed = true;
				}
				stream.EndInputSection();
				if (failed)
				{
					stream.DeactivateItem();
					break;
				}

				numElements = stream.BeginInputSection(L"ComparisonOperator");
				if (numElements)
				{
					stream.ActivateItem(0);
					stream.GetTextValue(value);
					caseDesc.comparisonOperator = static_cast<ComparisonOperator>(std::stoul(value));
					stream.DeactivateItem();
				}
				else
				{
					failed = true;
				}
				stream.EndInputSection();
				if (failed)
				{
					stream.DeactivateItem();
					break;
				}

				numElements = stream.BeginInputSection(L"RightOperandType");
				if (numElements)
				{
					stream.ActivateItem(0);
					stream.GetTextValue(value);
					caseDesc.rightOperandType = static_cast<OperandType>(std::stoul(value));
					stream.DeactivateItem();
				}
				else
				{
					failed = true;
				}
				stream.EndInputSection();
				if (failed)
				{
					stream.DeactivateItem();
					break;
				}

				numElements = stream.BeginInputSection(L"RightOperand");
				if (numElements)
				{
					stream.ActivateItem(0);
					stream.GetTextValue(caseDesc.rightOperand);
					stream.DeactivateItem();
				}
				else
				{
					failed = true;
				}
				stream.EndInputSection();
				if (failed)
				{
					stream.DeactivateItem();
					break;
				}
			}
			stream.DeactivateItem();

			mCases.push_back(caseDesc);
		}
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
