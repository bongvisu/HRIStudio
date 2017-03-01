#include "hsuiPrecompiled.h"
#include "hsuiPWSayBlock.h"
#include "hsuiVisualManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWSayBlock(Component* component)
{
	return new PWSayBlock(component);
}

void PWSayBlock::InitPWSayBlock()
{
	msFactory[XSC_RTTI(SayBlock).GetName()] = CreatePWSayBlock;
}

void PWSayBlock::TermPWSayBlock()
{
}
// ====================================================================================================================

PWSayBlock::PWSayBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWSayBlock::~PWSayBlock()
{
}

void PWSayBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"SAY - %s", mComponent->GetName().c_str());
}

void PWSayBlock::GetDimension(int& w, int& h) const
{
	w = 500;
	h = 201;
}

void PWSayBlock::OnInitWnd()
{
	auto block = static_cast<SayBlock*>(mComponent);

	int x = CONTENT_MARGIN;
	int y = TOP_MARGIN;

	mEdit.CreateWnd(this, x, y, 486, 164, false, false, false);
	mEdit.SetBorderColor(VisualManager::CLR_LEVEL3);

	mEdit.SetContent(block->GetCodeText());
}

void PWSayBlock::OnApply()
{
	auto block = static_cast<SayBlock*>(mComponent);

	block->SetCodeText(mEdit.GetContent());
}

} // namespace hsui
