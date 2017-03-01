#pragma once

#include <hsmoDialogBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiCodeEditor.h>

namespace hsui {

class PWDialogBlock : public ParamWindow
{
public:
	PWDialogBlock(xsc::Component* component);
	virtual ~PWDialogBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;

protected:
	CodeEditor mEdit;

	// ================================================================================================================
	private: friend Module;
	static void InitPWDialogBlock();
	static void TermPWDialogBlock();
	// ================================================================================================================
};

} // namespace hsui
