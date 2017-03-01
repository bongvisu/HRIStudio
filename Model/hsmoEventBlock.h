#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

class HSMO_API EventBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(EventBlock)

public:
	virtual ~EventBlock();

	void SetHriEvent(HriEvent hriEvent);
	HriEvent GetHriEvent() const;

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
	EventBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

protected:
	HriEvent mEvent;

	// ================================================================================================================
	private: friend Module;
	static void InitEventBlock();
	static void TermEventBlock();
	// ================================================================================================================
};

} // namespace hsmo
