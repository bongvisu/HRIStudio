#pragma once

#include <hsmoEventBlock.h>
#include <hsuiParamWindow.h>
#include <hsuiLabelCtrl.h>
#include <hsuiComboCtrl.h>
#include <hsuiTreeCtrl.h>
#include <hsuiListCtrl.h>

namespace hsui {

class PWEventBlock : public ParamWindow
{
public:
	PWEventBlock(xsc::Component* component);
	virtual ~PWEventBlock();

protected:
	virtual void BuilidTitleString() const override;
	virtual void GetDimension(int& w, int& h) const override;

	virtual void OnInitWnd() override;
	virtual void OnApply() override;
	virtual void OnPaintWnd() override;

protected:
	TreeCtrl mEvents;
	ListCtrl mAssociatedData;
	int mEventToItem[hsmo::HriEvent::NUMBER_OF_HRI_EVENTS];

	// ================================================================================================================
	private: friend Module;
	static void InitPWEventBlock();
	static void TermPWEventBlock();
	// ================================================================================================================
};

} // namespace hsui
