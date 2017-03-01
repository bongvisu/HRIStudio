#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

class HSMO_API DialogBlock : public HriBlock
{
	XSC_DECLARE_DYNCREATE(DialogBlock)

public:
	virtual ~DialogBlock();

protected:
	DialogBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

protected:

	// ================================================================================================================
	private: friend Module;
	static void InitDialogBlock();
	static void TermDialogBlock();
	// ================================================================================================================
};

} // namespace hsmo
