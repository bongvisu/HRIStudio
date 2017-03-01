#include "hsmnPrecompiled.h"
#include <afxwinappex.h>
#include <afxdialogex.h>
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"
#include "hsmnAboutBox.h"
#include "hsmnLevelView.h"
#include "hsmnNewModelDialog.h"
#include <hsmoModelManager.h>

#ifdef _DEBUG
#pragma comment(lib, "xsCored.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#pragma comment(lib, "HRIStudio.Presentation.lib")
#pragma comment(lib, "HRIStudio.UserInterface.lib")
#else
#pragma comment(lib, "xsCore.lib")
#pragma comment(lib, "HRIStudio.Model.lib")
#pragma comment(lib, "HRIStudio.Presentation.lib")
#pragma comment(lib, "HRIStudio.UserInterface.lib")
#endif // _DEBUG

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

Application theApp;

BEGIN_MESSAGE_MAP(Application, CWinAppEx)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

Application::Application()
{
	SetAppID(_T("HSU.HRI Studio.2"));
}

HINSTANCE Application::GetApplicationInstanceHandle() const
{
	return m_hInstance;
}

CString Application::GetApplicationTitle() const
{
	CString appTitle;
	appTitle.LoadString(theApp.m_hInstance, HSMN_IDR_MAIN_FRAME);
	return appTitle;
}

UINT Application::GetApplicationIconActive() const
{
	return HSMN_IDR_CAPTION_ICON_ACTIVE;
}

UINT Application::GetApplicationIconInactive() const
{
	return HSMN_IDR_CAPTION_ICON_INACTIVE;
}

bool Application::IsMdiActive() const
{
	return static_cast<MainFrame*>(m_pMainWnd)->IsMdiActive();
}

BOOL Application::InitInstance()
{
	INITCOMMONCONTROLSEX initCtrlsEx;
	initCtrlsEx.dwSize = sizeof(initCtrlsEx);
	initCtrlsEx.dwICC = ICC_WIN95_CLASSES | 0x0000FF00;
	InitCommonControlsEx(&initCtrlsEx);

	CWinAppEx::InitInstance();
	SetRegistryKey(_T("HSU"));

	// ----------------------------------------------------------------------------------------------------------------
	xsc::StartupModule();

	hsmo::Module::Init();
	hspr::Module::Init();
	hsui::Module::Init();

	LoadStateInfo();
	// ----------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
	theModelManager->OpenModel(LR"(C:\_WORLD_\HRI Studio\Test.hsproject)");
#endif // _DEBUG

	ITEMIDLIST* pidl;
	wchar_t specialFolder[MAX_PATH];
	SHGetSpecialFolderLocation(nullptr, CSIDL_COMMON_APPDATA, &pidl);
	SHGetPathFromIDListW(pidl, specialFolder);
	CoTaskMemFree(pidl);
	mAppDataDirectory = CString(specialFolder) + LR"(\HSU\HRI Studio 2.0\)";

	mTimer.Reset();

	//CleanState();

	MainFrame* mainFrame = new MainFrame;
	m_pMainWnd = mainFrame;
	mainFrame->LoadFrame(HSMN_IDR_MAIN_FRAME);
	mainFrame->ShowWindow(m_nCmdShow);
	mainFrame->UpdateWindow();

	return TRUE;
}

int Application::ExitInstance()
{
	// ----------------------------------------------------------------------------------------------------------------
	SaveStateInfo();

	hsui::Module::Term();
	hspr::Module::Term();
	hsmo::Module::Term();

	xsc::CleanupModule();
	// ----------------------------------------------------------------------------------------------------------------

	return CWinAppEx::ExitInstance();	
}

static const wchar_t* PLATFORM_SECTION = L"Platform";
static const wchar_t* PLATFORM_IP = L"IP";
static const wchar_t* PLATFORM_PORT = L"Port";
static const wchar_t* PLATFORM_ID = L"ID";
static const wchar_t* PLATFORM_PASSWORD = L"Password";
static const wchar_t* PLATFORM_DIRECTORY = L"Directory";

static const wchar_t* PRESENTATION_SECTION = L"Presentation";
static const wchar_t* PRESENTATION_GROUP_SELECTION_MODE = L"Group Selection Mode";

void Application::LoadStateInfo()
{
	auto strValue = theApp.GetProfileStringW(PLATFORM_SECTION, PLATFORM_IP, theModelManager->GetPlatformIP().c_str());
	theModelManager->SetPlatformIP((const wchar_t*)strValue);

	auto intValue = theApp.GetProfileIntW(
		PLATFORM_SECTION,
		PLATFORM_PORT,
		theModelManager->GetPlatformPort());
	theModelManager->SetPlatformPort(intValue);

	strValue = theApp.GetProfileStringW(
		PLATFORM_SECTION,
		PLATFORM_ID,
		theModelManager->GetPlatformID().c_str());
	theModelManager->SetPlatformID((const wchar_t*)strValue);

	strValue = theApp.GetProfileStringW(
		PLATFORM_SECTION,
		PLATFORM_PASSWORD,
		theModelManager->GetPlatformID().c_str());
	theModelManager->SetPlatformPassword((const wchar_t*)strValue);

	strValue = theApp.GetProfileStringW(
		PLATFORM_SECTION,
		PLATFORM_DIRECTORY,
		theModelManager->GetPlatformDirectory().c_str());
	theModelManager->SetPlatformDirectory((const wchar_t*)strValue);

	DHSelectGroup::groupSelectionMode = static_cast<GroupSelectionMode>(theApp.GetProfileIntW(
		PRESENTATION_SECTION, PRESENTATION_GROUP_SELECTION_MODE, DHSelectGroup::groupSelectionMode));
}

void Application::SaveStateInfo()
{
	auto& ip = theModelManager->GetPlatformIP();
	theApp.WriteProfileStringW(PLATFORM_SECTION, PLATFORM_IP, ip.c_str());

	auto port = theModelManager->GetPlatformPort();
	theApp.WriteProfileInt(PLATFORM_SECTION, PLATFORM_PORT, port);

	auto& id = theModelManager->GetPlatformID();
	theApp.WriteProfileStringW(PLATFORM_SECTION, PLATFORM_ID, id.c_str());

	auto& password = theModelManager->GetPlatformPassword();
	theApp.WriteProfileStringW(PLATFORM_SECTION, PLATFORM_PASSWORD, password.c_str());

	auto& directory = theModelManager->GetPlatformDirectory();
	theApp.WriteProfileStringW(PLATFORM_SECTION, PLATFORM_DIRECTORY, directory.c_str());

	theApp.WriteProfileInt(PRESENTATION_SECTION, PRESENTATION_GROUP_SELECTION_MODE, DHSelectGroup::groupSelectionMode);
}

void Application::OnFileNew() 
{
	NewModelDialog dlg;
	dlg.DoModal();
}

void Application::OnFileOpen()
{
	CString filter(L"HRI Studio Project|*");
	filter += Model::FILE_EXTENSION;
	filter += L"|";
	CFileDialog fileDlg(TRUE, 0, 0, OFN_ENABLESIZING, filter);
	if (IDOK == fileDlg.DoModal())
	{
		wstring pathName(fileDlg.GetPathName().GetBuffer());
		theModelManager->OpenModel(pathName.c_str());
	}
}

void Application::OnFileSave()
{
	theModelManager->Save();
}

void Application::OnFileClose()
{
	theModelManager->CloseActiveModel();
}

void Application::LoadCustomState()
{
}

void Application::SaveCustomState()
{
}

void Application::OnAppAbout()
{
	AboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void Application::OnHelp()
{
	ShellExecuteW(nullptr, L"open", L"iexplore.exe", L"http://www.hansung.ac.kr/~hkcho", nullptr, SW_SHOW);
}

void Application::PreLoadState()
{
	GetContextMenuManager()->AddMenu(L"Project", HSMN_IDR_PROJECT_POPUP);
}

void Application::UpdateLoopConsumers()
{
	double t = mTimer.GetSeconds();
	for (const auto view : mLoopConsumers)
	{
		view->DoLoop(t);
	}
	Sleep(0);
}

BOOL Application::OnIdle(LONG lCount)
{
	CWinAppEx::OnIdle(lCount);
	
	UpdateLoopConsumers();

	return TRUE;
}

double Application::GetAppTime() const
{
	return mTimer.GetSeconds();
}

void Application::RegisterLoopConsumer(ILoopConsumer* updateTarget)
{
	mLoopConsumers.insert(updateTarget);
}

void Application::UnregisterLoopConsumer(ILoopConsumer* updateTarget)
{
	mLoopConsumers.erase(updateTarget);
}

const CString& Application::GetAppDataDirectory() const
{
	return mAppDataDirectory;
}

} // namespace hsmn
