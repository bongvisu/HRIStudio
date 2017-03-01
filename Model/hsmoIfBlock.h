#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

class HSMO_API IfBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(IfBlock)

public:
	virtual ~IfBlock();

	void SetLeftOperandType(OperandType operandType);
	OperandType GetLeftOperandType() const;

	void SetLeftOperand(const std::wstring& operand);
	const std::wstring& GetLeftOperand() const;

	void SetComparisonOperator(ComparisonOperator comparisonOperator);
	ComparisonOperator GetComparisonOperator() const;

	void SetRightOperandType(OperandType operandType);
	OperandType GetRightOperandType() const;

	void SetRightOperand(const std::wstring& value);
	const std::wstring& GetRightOperand() const;

	// Code Generation ------------------------------------------------------------------------------------------------
	virtual std::wstring GetEntryCode() const override;
	virtual bool GenerateCode(std::wstring& code, int level) override;
	// ----------------------------------------------------------------------------------------------------------------

	// Cloning --------------------------------------------------------------------------------------------------------
	virtual Component* Clone(xsc::ComponentToComponent& remapper) override;
	// ----------------------------------------------------------------------------------------------------------------

	// Stream ---------------------------------------------------------------------------------------------------------
	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	// ----------------------------------------------------------------------------------------------------------------

protected:
	IfBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

protected:
	OperandType mLeftOperandType;
	std::wstring mLeftOperand;
	
	ComparisonOperator mComparisonOperator;

	OperandType mRightOperandType;
	std::wstring mRightOperand;

	// ================================================================================================================
	private: friend Module;
	static void InitIfBlock();
	static void TermIfBlock();
	// ================================================================================================================
};

} // namespace hsmo
