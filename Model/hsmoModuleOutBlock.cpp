#include "hsmoPrecompiled.h"
#include "hsmoModuleOutBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleOutBlock::InitModuleOutBlock()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleOutBlock)
}

void ModuleOutBlock::TermModuleOutBlock()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleOutBlock, ModuleIOBlock)

ModuleOutBlock::ModuleOutBlock()
{
	mName = L"Output";
}

ModuleOutBlock::~ModuleOutBlock()
{
}

void ModuleOutBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddInputPort(MPRectangle::EDGE_L);
}

PortType ModuleOutBlock::GetCompatiblePortType() const
{
	return PORT_TYPE_OUTPUT;
}

} // namespace hsmo
