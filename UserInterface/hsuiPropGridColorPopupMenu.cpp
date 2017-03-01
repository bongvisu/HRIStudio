#include "hsuiPrecompiled.h"
#include "hsuiPropGridColorPopupMenu.h"

namespace hsui {

IMPLEMENT_DYNAMIC(PropGridColorPopupMenu, CMFCPopupMenu)

BEGIN_MESSAGE_MAP(PropGridColorPopupMenu, CMFCPopupMenu)
	ON_WM_CREATE()
END_MESSAGE_MAP()

PropGridColorPopupMenu::~PropGridColorPopupMenu()
{
}

int PropGridColorPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCToolBar::IsCustomizeMode())
	{
		// Don't show color popup in customization mode
		return -1;
	}

	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	DWORD toolbarStyle = AFX_DEFAULT_TOOLBAR_STYLE;
	if (GetAnimationType() != NO_ANIMATION && !CMFCToolBar::IsCustomizeMode())
	{
		toolbarStyle &= ~WS_VISIBLE;
	}

	if (!mColorBar.Create(this, toolbarStyle | CBRS_TOOLTIPS | CBRS_FLYBY, 1))
	{
		TRACE(_T("Can't create popup menu bar\n"));
		return -1;
	}

	CWnd* pWndParent = GetParent();
	ASSERT_VALID(pWndParent);

	mColorBar.SetOwner(pWndParent);
	mColorBar.SetPaneStyle(mColorBar.GetPaneStyle() | CBRS_TOOLTIPS);

	CFrameWnd* topLevelFrame = pWndParent->GetTopLevelFrame();
	ActivatePopupMenu(topLevelFrame, this);
	RecalcLayout();
	return 0;
}

CPane* PropGridColorPopupMenu::CreateTearOffBar(CFrameWnd* pWndMain, UINT uiID, LPCTSTR lpszName)
{
	ASSERT_VALID(pWndMain);
	ENSURE(lpszName != nullptr);
	ENSURE(uiID != 0);

	CMFCColorMenuButton* colorMenuButton = DYNAMIC_DOWNCAST(CMFCColorMenuButton, GetParentButton());
	if (colorMenuButton == nullptr)
	{
		ASSERT(FALSE);
		return nullptr;
	}

	PropGridColorBar* pColorBar = new PropGridColorBar(mColorBar, colorMenuButton->m_nID);

	if (!pColorBar->Create(pWndMain, AFX_DEFAULT_TOOLBAR_STYLE, uiID))
	{
		TRACE0("Failed to create a new toolbar!\n");
		delete pColorBar;
		return nullptr;
	}

	pColorBar->SetWindowText(lpszName);
	pColorBar->SetPaneStyle(pColorBar->GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	pColorBar->EnableDocking(CBRS_ALIGN_ANY);

	return pColorBar;
}

} // namespace hsui
