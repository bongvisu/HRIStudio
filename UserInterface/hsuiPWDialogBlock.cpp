#include "hsuiPrecompiled.h"
#include "hsuiPWDialogBlock.h"
#include "hsuiVisualManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWDialogBlock(Component* component)
{
	return new PWDialogBlock(component);
}

void PWDialogBlock::InitPWDialogBlock()
{
	msFactory[XSC_RTTI(DialogBlock).GetName()] = CreatePWDialogBlock;
}

void PWDialogBlock::TermPWDialogBlock()
{
}
// ====================================================================================================================

PWDialogBlock::PWDialogBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWDialogBlock::~PWDialogBlock()
{
}

void PWDialogBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"DIALOG - %s", mComponent->GetName().c_str());
}

void PWDialogBlock::GetDimension(int& w, int& h) const
{
	w = 500;
	h = 201;
}

void PWDialogBlock::OnInitWnd()
{
	auto block = static_cast<DialogBlock*>(mComponent);

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;

	mEdit.CreateWnd(this, x, y, 486, 164, false, false, false);
	mEdit.SetBorderColor(VisualManager::CLR_LEVEL3);
}

void PWDialogBlock::OnApply()
{
	auto block = static_cast<DialogBlock*>(mComponent);
}

} // namespace hsui
