#include "hsprPrecompiled.h"

#include "hsprLinkPresenter.h"
#include "hsprBPModuleBlock.h"
#include "hsprBPModuleInBlock.h"
#include "hsprBPModuleOutBlock.h"
#include "hsprBPEventBlock.h"
#include "hsprBPIfBlock.h"
#include "hsprBPSwitchBlock.h"
#include "hsprBPSayBlock.h"
#include "hsprBPVariableBlock.h"
#include "hsprBPDialogBlock.h"

#ifdef _DEBUG
#pragma comment(lib, "xsCored.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#else
#pragma comment(lib, "xsCore.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#endif // _DEBUG

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

HMODULE Module::handle;

void Module::Init()
{
	Presenter::InitPresenter();
	IPointSnapTarget::InitIPointSnapTarget();
	PortPresenter::InitPortPresenter();
	SegmentPresenter::InitSegmentPresenter();
	LinkPresenter::InitLinkPresenter();

	BPModuleBlock::InitBPModuleBlock();
	BPModuleInBlock::InitBPModuleInBlock();
	BPModuleOutBlock::InitBPModuleOutBlock();

	BPIfBlock::InitBPIfBlock();
	BPSwitchBlock::InitBPSwitchBlock();
	BPSayBlock::InitBPSayBlock();
	BPEventBlock::InitBPEventBlock();
	BPVariableBlock::InitBPVariableBlock();
	BPDialogBlock::InitBPDialogBlock();
}

void Module::Term()
{
	BPDialogBlock::TermBPDialogBlock();
	BPVariableBlock::TermBPVariableBlock();
	BPEventBlock::TermBPEventBlock();
	BPSayBlock::TermBPSayBlock();
	BPSwitchBlock::TermBPSwitchBlock();
	BPIfBlock::TermBPIfBlock();

	BPModuleOutBlock::TermBPModuleOutBlock();
	BPModuleInBlock::TermBPModuleInBlock();
	BPModuleBlock::TermBPModuleBlock();
	
	LinkPresenter::TermLinkPresenter();
	SegmentPresenter::TermSegmentPresenter();
	PortPresenter::TermPortPresenter();
	IPointSnapTarget::TermIPointSnapTarget();
	Presenter::TermPresenter();
}

} // namespace hspr

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD, LPVOID)
{
	hspr::Module::handle = moduleHandle;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
