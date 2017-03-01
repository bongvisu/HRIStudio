#include "hsuiPrecompiled.h"
#include "hsuiPropHandler.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PropHandler::GetDisplayType() const
{
	return L"";
}

void PropHandler::Setup()
{
	// to be overriden.
}

void PropHandler::ReflectChange(Entity::PropID prop)
{
	// to be overriden.
}

} // namespace hsui
