#include "hsuiPrecompiled.h"
#include "hsuiVisualManager.h"
#include "hsuiPropManager.h"
#include "hsuiParamManager.h"
#include "hsuiEditCtrl.h"
#include "hsuiToolTipCtrl.h"
#include "hsuiShadowBorder.h"
#include "hsuiResource.h"

#include "hsuiPWLink.h"
#include "hsuiPWModel.h"
#include "hsuiPWIfBlock.h"
#include "hsuiPWSayBlock.h"
#include "hsuiPWEventBlock.h"
#include "hsuiPWSwitchBlock.h"
#include "hsuiPWVariableBlock.h"
#include "hsuiPWDialogBlock.h"

#ifdef _DEBUG
#pragma comment(lib, "xsCored.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#else
#pragma comment(lib, "xsCore.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#endif // _DEBUG

namespace hsui {

HMODULE Module::handle;

void Module::Init()
{
	AfxOleInit();
	AfxEnableControlContainer();
	AfxInitRichEdit2();

	CWinAppEx* theApp = static_cast<CWinAppEx*>(AfxGetApp());

	GetGlobalData()->m_hcurMoveTab = theApp->LoadCursorW(HSUI_IDR_TAB_MOVE_CURSOR_E);
	GetGlobalData()->m_hcurNoMoveTab = theApp->LoadCursorW(HSUI_IDR_TAB_MOVE_CURSOR_D);
	GetGlobalData()->m_hcurStretch = AfxGetApp()->LoadCursor(HSUI_IDR_RESIZE_CURSOR_H);
	GetGlobalData()->m_hcurStretchVert = AfxGetApp()->LoadCursor(HSUI_IDR_RESIZE_CURSOR_V);

	theApp->EnableTaskbarInteraction(FALSE);

	theApp->InitContextMenuManager();

	theApp->InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bBoldLabel = TRUE;
	ttParams.m_bVislManagerTheme = FALSE;
	ttParams.m_clrBorder = VisualManager::ToGdiColor(VisualManager::CLR_LEVEL2);
	ttParams.m_clrFill = VisualManager::ToGdiColor(VisualManager::CLR_BASE);
	theApp->GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(ToolTipCtrl), &ttParams);

	VisualManager::InitVisualManager();
	EditCtrl::InitEditCtrl();
	ShadowBorder::InitShadowBorder();

	// ----------------------------------------------------------------------------------------------------------------
	ParamWindow::InitParamWindow();

	PWLink::InitPWLink();
	PWModel::InitPWModel();

	PWIfBlock::InitPWIfBlock();
	PWSayBlock::InitPWSayBlock();
	PWEventBlock::InitPWEventBlock();
	PWSwitchBlock::InitPWSwitchBlock();
	PWVariableBlock::InitPWVariableBlock();
	PWDialogBlock::InitPWDialogBlock();
	// ----------------------------------------------------------------------------------------------------------------

	PropManager::InitPropManager();
	ParamManager::InitParamManager();
}

void Module::Term()
{
	ParamManager::TermParamManager();
	PropManager::TermPropManager();
	
	// ----------------------------------------------------------------------------------------------------------------
	PWDialogBlock::TermPWDialogBlock();
	PWVariableBlock::TermPWVariableBlock();
	PWSwitchBlock::TermPWSwitchBlock();
	PWEventBlock::TermPWEventBlock();
	PWSayBlock::TermPWSayBlock();
	PWIfBlock::TermPWIfBlock();

	PWModel::TermPWModel();
	PWLink::TermPWLink();

	ParamWindow::TermParamWindow();
	// ----------------------------------------------------------------------------------------------------------------

	ShadowBorder::TermShadowBorder();
	EditCtrl::TermEditCtrl();
	VisualManager::TermVisualManager();

	AfxOleTerm(FALSE);
}

} // namespace hsui

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static AFX_EXTENSION_MODULE afxModule = { FALSE, nullptr };
extern "C" int APIENTRY DllMain(HINSTANCE hModule, DWORD reasonForCall, LPVOID)
{
	hsui::Module::handle = hModule;

	if (reasonForCall == DLL_PROCESS_ATTACH)
	{
		AfxInitExtensionModule(afxModule, hModule);
		new CDynLinkLibrary(afxModule);	
	}
	else if (reasonForCall == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(afxModule);
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
