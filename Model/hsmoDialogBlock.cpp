#include "hsmoPrecompiled.h"
#include "hsmoDialogBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNCREATE(hsmo, DialogBlock, HriBlock)

// ====================================================================================================================
void DialogBlock::InitDialogBlock()
{
	XSC_REGISTER_FACTORY(hsmo, DialogBlock)
}

void DialogBlock::TermDialogBlock()
{
}
// ====================================================================================================================

DialogBlock::DialogBlock()
{
	mName = L"Dialog";
}

DialogBlock::~DialogBlock()
{
}

void DialogBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddInputPort(MPRectangle::EDGE_L);
	AddOutputPort(MPRectangle::EDGE_R);
}

} // namespace hsmo
