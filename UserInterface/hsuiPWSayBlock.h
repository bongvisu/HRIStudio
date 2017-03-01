#pragma once

#include <hsmoSayBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiCodeEditor.h>

namespace hsui {

class PWSayBlock : public ParamWindow
{
public:
	PWSayBlock(xsc::Component* component);
	virtual ~PWSayBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;
	
	virtual void OnInitWnd() override;
	virtual void OnApply() override;

protected:
	CodeEditor mEdit;

	// ================================================================================================================
	private: friend Module;
	static void InitPWSayBlock();
	static void TermPWSayBlock();
	// ================================================================================================================
};

} // namespace hsui
