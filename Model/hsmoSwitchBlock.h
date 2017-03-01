#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

struct HSMO_API CaseDescription
{
public:
	CaseDescription();
	
	CaseDescription(const CaseDescription& caseDescription);
	CaseDescription& operator=(const CaseDescription& caseDescription);

	CaseDescription(CaseDescription&& caseDescription);
	CaseDescription& operator=(CaseDescription&& caseDescription);
	
public:
	OperandType				leftOperandType;
	std::wstring			leftOperand;

	ComparisonOperator		comparisonOperator;

	OperandType				rightOperandType;
	std::wstring			rightOperand;
};

typedef std::vector<CaseDescription> CaseDescriptionList;

class HSMO_API SwitchBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(SwitchBlock)

public:
	static const unsigned int MAX_NUM_CASES = 5;
	static const unsigned int MIN_NUM_CASES = 2;

public:
	virtual ~SwitchBlock();

	void SetCases(CaseDescriptionList&& cases);
	const CaseDescriptionList& GetCases() const;

	// Code Generation ------------------------------------------------------------------------------------------------
	virtual std::wstring GetEntryCode() const override;
	virtual bool GenerateCode(std::wstring& code, int level) override;
	// ----------------------------------------------------------------------------------------------------------------

	// Memento --------------------------------------------------------------------------------------------------------
	class SwitchBlockMemento : public BlockMemento
	{
	public:
		SwitchBlockMemento(SwitchBlock* block);
		virtual void Restore() override;

	protected:
		std::vector<CaseDescription> mCases;
	};
	virtual xsc::MementoPtr SaveState() const override;
	// ----------------------------------------------------------------------------------------------------------------
	
	// Cloning --------------------------------------------------------------------------------------------------------
	virtual Component* Clone(xsc::ComponentToComponent& remapper) override;
	// ----------------------------------------------------------------------------------------------------------------

	// Stream ---------------------------------------------------------------------------------------------------------
	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	// ----------------------------------------------------------------------------------------------------------------

protected:
	SwitchBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

protected:
	std::vector<CaseDescription> mCases;

	// ================================================================================================================
	private: friend Module;
	static void InitSwitchBlock();
	static void TermSwitchBlock();
	// ================================================================================================================
};

} // namespace hsmo
