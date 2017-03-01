#include "hsmnPrecompiled.h"
#include "hsmnViewFrame.h"
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(ViewFrame, CMDIChildWndEx)
	ON_WM_MDIACTIVATE()
END_MESSAGE_MAP()

ViewFrame::ViewFrame()
{
}

ViewFrame::~ViewFrame()
{
}

wstring ViewFrame::GetNameText() const
{
	return L"";
}

wstring ViewFrame::GetPathText() const
{
	return L"";
}

BOOL ViewFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	CMDIChildWndEx::PreCreateWindow(cs);

	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

void ViewFrame::OnMDIActivate(BOOL activate, CWnd* activateWnd, CWnd* deactivateWnd)
{
	static_cast<MainFrame*>(theApp.m_pMainWnd)->RedrawMdiTabAreas();
	CMDIChildWndEx::OnMDIActivate(activate, activateWnd, deactivateWnd);
}

} // namespace hsmn
