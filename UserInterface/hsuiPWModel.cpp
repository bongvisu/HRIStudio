#include "hsuiPrecompiled.h"
#include "hsuiPWModel.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWModel(Component* component)
{
	return new PWModel(component);
}

void PWModel::InitPWModel()
{
	msFactory[XSC_RTTI(Model).GetName()] = CreatePWModel;
}

void PWModel::TermPWModel()
{
}
// ====================================================================================================================

PWModel::PWModel(Component* component)
	:
	ParamWindow(component)
{
}

PWModel::~PWModel()
{
}

void PWModel::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"PROJECT - %s", mComponent->GetName().c_str());
}

} // namespace hsui
