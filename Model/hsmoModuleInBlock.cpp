#include "hsmoPrecompiled.h"
#include "hsmoModuleInBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleInBlock::InitModuleInBlock()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleInBlock)
}

void ModuleInBlock::TermModuleInBlock()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleInBlock, ModuleIOBlock)

ModuleInBlock::ModuleInBlock()
{
	mName = L"Input";
}

ModuleInBlock::~ModuleInBlock()
{
}

void ModuleInBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddOutputPort(MPRectangle::EDGE_R);
}

PortType ModuleInBlock::GetCompatiblePortType() const
{
	return PORT_TYPE_INPUT;
}

} // namespace hsmo
