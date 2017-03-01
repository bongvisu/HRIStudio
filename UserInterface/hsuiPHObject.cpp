#include "hsuiPrecompiled.h"
#include "hsuiPHObject.h"
#include "hsuiPropManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

void PHObject::Setup()
{
	PropGridCtrl& view = thePropManager->GetView();
	view.RemoveAll();
	view.Invalidate(FALSE);
}

} // namespace hsui
