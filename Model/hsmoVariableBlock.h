#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

enum VariableOperation
{
	// Assign
	VARIABLE_OPERATION_ASSIGN = 0,

	// Arithmetic
	VARIABLE_OPERATION_ADDITION ,
	VARIABLE_OPERATION_SUBTRACTION,
	VARIABLE_OPERATION_MULTIPLICATION,
	VARIABLE_OPERATION_DIVISION,

	NUMBER_OF_VARIABLE_OPERATIONS
};


class HSMO_API VariableBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(VariableBlock)

public:
	virtual ~VariableBlock();

	void SetVariable(const std::wstring& variable);
	const std::wstring& GetVariable() const;

	void SetOperation(VariableOperation operation);
	VariableOperation GetOperation() const;

	void SetExpression(const std::wstring& expression);
	const std::wstring& GetExpression() const;

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
	VariableBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

	std::wstring mVariable;
	VariableOperation mOperation;
	std::wstring mExpression;

	// ================================================================================================================
	private: friend Module;
	static void InitVariableBlock();
	static void TermVariableBlock();
	// ================================================================================================================
};

} // namespace hsmo
