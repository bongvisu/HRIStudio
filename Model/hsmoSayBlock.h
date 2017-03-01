#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

class HSMO_API SayBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(SayBlock)

public:
	virtual ~SayBlock();

	void SetCodeText(const std::wstring& text);
	const std::wstring& GetCodeText() const;

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
	SayBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

protected:
	std::wstring mCodeText;

	// ================================================================================================================
	private: friend Module;
	static void InitSayBlock();
	static void TermSayBlock();
	// ================================================================================================================
};

} // namespace hsmo
