#include "hsmoPrecompiled.h"
#include "hsmoLogMediator.h"

#include "hsmoModuleBlock.h"
#include "hsmoEventBlock.h"
#include "hsmoSayBlock.h"
#include "hsmoIfBlock.h"
#include "hsmoSwitchBlock.h"
#include "hsmoVariableBlock.h"
#include "hsmoDialogBlock.h"

#include "hsmoModelManager.h"

#ifdef _DEBUG
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "ChilkatDbgDll_x64.lib")
#pragma comment(lib, "cpprest140d_2_9.lib")
#pragma comment(lib, "xsCored.lib")
#else
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "ChilkatRelDll_x64.lib")
#pragma comment(lib, "cpprest140_2_9.lib")
#pragma comment(lib, "xsCore.lib")
#endif // _DEBUG

using namespace xsc;

namespace hsmo {

HMODULE Module::handle;

void Module::Init()
{
	LogMediator::InitLogMediator();

	// ----------------------------------------------------------------------------------------------------------------
	ModuleBlock::InitModuleBlock();
	ModuleInPort::InitModuleInPort();
	ModuleOutPort::InitModuleOutPort();
	ModuleLevel::InitModuleLevel();
	ModuleInBlock::InitModuleInBlock();
	ModuleOutBlock::InitModuleOutBlock();

	EventBlock::InitEventBlock();
	IfBlock::InitIfBlock();
	SayBlock::InitSayBlock();
	SwitchBlock::InitSwitchBlock();
	VariableBlock::InitVariableBlock();
	DialogBlock::InitDialogBlock();
	// ----------------------------------------------------------------------------------------------------------------

	ModelManager::InitModelManager();
}

void Module::Term()
{
	ModelManager::TermModelManager();

	// ----------------------------------------------------------------------------------------------------------------
	DialogBlock::TermDialogBlock();
	VariableBlock::InitVariableBlock();
	SwitchBlock::InitSwitchBlock();
	SayBlock::InitSayBlock();
	IfBlock::InitIfBlock();
	EventBlock::InitEventBlock();

	ModuleOutBlock::InitModuleOutBlock();
	ModuleInBlock::InitModuleInBlock();
	ModuleLevel::InitModuleLevel();
	ModuleOutPort::InitModuleOutPort();
	ModuleInPort::InitModuleInPort();
	ModuleBlock::InitModuleBlock();
	// ----------------------------------------------------------------------------------------------------------------

	LogMediator::TermLogMediator();
}

} // namespace hsmo

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD, LPVOID)
{
	hsmo::Module::handle = moduleHandle;
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
