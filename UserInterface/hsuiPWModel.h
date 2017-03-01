#pragma once

#include <hsmoModel.h>
#include <hsuiParamWindow.h>

namespace hsui {

class PWModel : public ParamWindow
{
public:
	PWModel(xsc::Component* component);
	virtual ~PWModel();

protected:
	virtual void BuilidTitleString() const override;

	// ================================================================================================================
	private: friend Module;
	static void InitPWModel();
	static void TermPWModel();
	// ================================================================================================================
};

} // namespace hsui
