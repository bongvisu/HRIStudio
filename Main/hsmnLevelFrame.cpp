#include "hsmnPrecompiled.h"
#include "hsmnLevelFrame.h"
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"
#include "hsmnLevelPanel.h"

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(LevelFrame, ViewFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

LevelFrame::LevelFrame(xsc::Level* level, CMDIFrameWnd* parent)
	:
	mLevel(level)
{
	DWORD style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, L"", style, rectDefault, parent);
	InitialUpdateFrame(nullptr, TRUE);
}

LevelFrame::~LevelFrame()
{
}

Level* LevelFrame::GetLevel() const
{
	return mLevel;
}

wstring LevelFrame::GetNameText() const
{
	return mLevel->GetName();
}

wstring LevelFrame::GetPathText() const
{
	wstring path;
	mLevel->GetHierarchicalPath(path);
	return path;
}

int LevelFrame::OnCreate(LPCREATESTRUCT createStruct)
{
	ViewFrame::OnCreate(createStruct);

	mPanel = make_unique<LevelPanel>(mLevel, this);

	return 0;
}

void LevelFrame::OnDestroy() 
{
	MainFrame* mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
	mainFrame->EraseLevelView(mLevel);

	mPanel->DestroyWindow();
	mPanel.reset();

	ViewFrame::OnDestroy();
}

void LevelFrame::OnClose()
{
	ViewFrame::OnClose();
}

void LevelFrame::OnSetFocus(CWnd* pOldWnd) 
{
	ViewFrame::OnSetFocus(pOldWnd);

	if (mPanel)
	{
		mPanel->SetFocus();
	}
}

BOOL LevelFrame::OnCmdMsg(UINT id, int code, void* extra, AFX_CMDHANDLERINFO* handlerInfo)
{
	if (mPanel->OnCmdMsg(id, code, extra, handlerInfo))
	{
		return TRUE;
	}
	return ViewFrame::OnCmdMsg(id, code, extra, handlerInfo);
}

} // namespace hsmn
