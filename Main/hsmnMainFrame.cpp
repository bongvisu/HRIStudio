#include "hsmnPrecompiled.h"
#include "hsmnMainFrame.h"
#include "hsmnApplication.h"
#include "hsmnLevelFrame.h"
#include "hsmnWindowDialog.h"
#include "hsmnCodeView.h"
#include "hsmnTagAuthorView.h"
#include "hsmnSettingsDialog.h"
#include <xscDWrite.h>
#include <hsmoModelManager.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int FRAME_BORDER_SIZE	= 3;
static const int CAPTION_HEIGHT		= 30;
static const int STATUS_HEIGHT		= 25;
static const int INFO_GADGET_SIZE	= 400;

static const UINT WND_ID_PROJECT_PANE		= 1000;
static const UINT WND_ID_LIBRARY_PANE		= 1001;
static const UINT WND_ID_PROPERTY_PANE		= 1002;
static const UINT WND_ID_LOG_PANE			= 1003;
static const UINT WND_ID_MONITORING_PANE	= 1004;

static const int MIN_WND_WIDTH	= 1024;
static const int MIN_WND_HEIGHT = 768;

static const int FIXED_STATUS_GADGET_HEIGHT = 23;

static const UINT INITIAL_MODEL_OPEN_TIMER	= 1000;
static const UINT DELAYED_REDRAW_TIMER		= 1001;
static const UINT UDATE_MDI_TAB_TIMER		= 1002;

IMPLEMENT_DYNAMIC(MainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(MainFrame, CMDIFrameWndEx)
	ON_WM_GETMINMAXINFO()

	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCACTIVATE()
	ON_WM_WINDOWPOSCHANGED()

	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	
	ON_COMMAND(HSMN_IDC_VIEW_PROJECT_PANE, OnViewProjectPane)
	ON_COMMAND(HSMN_IDC_VIEW_LIBRARY_PANE, OnViewLibraryPane)
	ON_COMMAND(HSMN_IDC_VIEW_PROPERTY_PANE, OnViewPropertyPane)
	ON_COMMAND(HSMN_IDC_VIEW_LOG_PANE, OnViewLogPane)
	ON_COMMAND(HSMN_IDC_VIEW_MONITORING_PANE, OnViewMonitoringPane)
	
	ON_COMMAND(HSMN_IDC_PROJECT_COMPILE, OnProjectCompile)
	ON_COMMAND(HSMN_IDC_PROJECT_DOWNLOAD, OnProjectDownload)
	ON_COMMAND(HSMN_IDC_TOOL_SETTINGS, OnToolSettings)
	ON_COMMAND(HSMN_IDC_TOOL_TAG_AUTOR, OnToolTagAuthor)

	ON_WM_INITMENUPOPUP()
	ON_COMMAND(HSMN_IDC_WINDOWS_CLOSE_ALL, OnWindowCloseAll)
	ON_COMMAND(HSMN_IDC_WINDOWS_MANAGER, OnWindowManager)
	ON_COMMAND_RANGE(HSMN_IDC_WINDOWS_CLOSE_ALL + 1, HSMN_IDC_WINDOWS_CLOSE_ALL + 10, OnActivateWindow)

	ON_UPDATE_COMMAND_UI(HSMN_IDC_VIEW_PROJECT_PANE, OnViewProjectPaneUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(HSMN_IDC_VIEW_LIBRARY_PANE, OnViewLibraryPaneUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(HSMN_IDC_VIEW_PROPERTY_PANE, OnViewPropertyPaneUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(HSMN_IDC_VIEW_LOG_PANE, OnViewLogPaneUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(HSMN_IDC_VIEW_MONITORING_PANE, OnViewMonitoringPaneUpdateCmdUI)
	
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)

	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

MainFrame::MainFrame()
	:
	mBrush(nullptr),
	mIconActive(nullptr),
	mIconInactive(nullptr),
	mMaximized(false),
	mBorderSize(-1)
{
	mMdiActive = false;
	mMovingTab = false;
	mResizingTab = false;
	mTrackingTabMouseLeave = false;

	mCodeView = nullptr;
	mTagAuthorView = nullptr;
}

MainFrame::~MainFrame()
{
}

BOOL MainFrame::LoadFrame(UINT resourceID, DWORD defaultStyle, CWnd* parentWnd, CCreateContext* context)
{
	CMDIFrameWndEx::LoadFrame(resourceID, defaultStyle, parentWnd, context);

	if (theModelManager->GetNumModels())
	{
		SetTimer(INITIAL_MODEL_OPEN_TIMER, 10, nullptr);
	}

	mProjectPane.SetFocus();

	return TRUE;
}

void MainFrame::OnNotiChildAddition(Component* sender, const ComponentSet& components)
{
	if (sender == theModelManager)
	{
		LockWindowUpdate();

		for (auto component : components)
		{
			if (component->IsDerivedFrom(XSC_RTTI(Model)))
			{
				ViewFrame* viewFrame = nullptr;
				for (auto cvPair : mViews)
				{
					if (cvPair.first == component)
					{
						viewFrame = cvPair.second;
						break;
					}
				}

				if (viewFrame)
				{
					MDIActivate(viewFrame);
				}
				else
				{
					auto levelFrame = new LevelFrame(static_cast<Level*>(component), this);
					mViews.push_back({ component, levelFrame });
				}
			}
		}

		UnlockWindowUpdate();
	}
}

void MainFrame::OnNotiChildRemoval(Component*, const ComponentSet& components)
{
	mTempViewFrameVec.clear();
	for (auto cvPair : mViews)
	{
		for (auto component : components)
		{
			if (component->Contains(cvPair.first))
			{
				mTempViewFrameVec.push_back(cvPair.second);
				break;
			}
		}
	}

	for (auto viewFrame : mTempViewFrameVec)
	{
		viewFrame->DestroyWindow();
	}
}

void MainFrame::OnNotiPropertyChange(Component* sender, Entity::PropID prop)
{
	if (sender == theModelManager)
	{
		if (prop == ModelManager::PROP_ACTIVE_MODEL)
		{
			Model* activeModel = theModelManager->GetActiveModel();
			if (activeModel)
			{
				CString wndText(activeModel->GetName().c_str());
				wndText += L" - HRI Studio";
				SetWindowTextW(wndText);
			}
			else
			{
				SetWindowTextW(L"HRI Studio");
			}
			Invalidate(FALSE);
		}
	}
	else if (sender->IsDerivedFrom(XSC_RTTI(Model)))
	{
		if (prop == Component::PROP_NAME)
		{
			if (sender == theModelManager->GetActiveModel())
			{
				Invalidate(FALSE);
			}
		}
	}
}

bool MainFrame::IsRenderable(LevelView* levelView) const
{
	if (mMovingTab || mResizingTab)
	{
		return false;
	}

	CFrameWnd* levelFrame = levelView->GetParentFrame();
	return IsFrameVisible(static_cast<ViewFrame*>(levelFrame));
}

void MainFrame::EraseLevelView(Level* level)
{
	list<ComponetViewPair>::const_iterator iter;
	for (iter = mViews.begin(); iter != mViews.end(); ++iter)
	{
		if (iter->first == level)
		{
			mViews.erase(iter);
			break;
		}
	}
}

void MainFrame::OpenLevelView(Level* level)
{
	LockWindowUpdate();

	ViewFrame* viewFrame = nullptr;
	for (auto cvPair : mViews)
	{
		if (cvPair.first == level)
		{
			viewFrame = cvPair.second;
			break;
		}
	}

	if (viewFrame)
	{
		MDIActivate(viewFrame);
	}
	else
	{
		LevelFrame* levelFrame = new LevelFrame(static_cast<Level*>(level), this);
		mViews.push_back({ level, levelFrame });
	}

	UnlockWindowUpdate();
}

void MainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CMDIFrameWndEx::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = MIN_WND_WIDTH;
	lpMMI->ptMinTrackSize.y = MIN_WND_HEIGHT;
}

void MainFrame::OnNcCalcSize(BOOL calcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	RECT rect = lpncsp->rgrc[0];

	if (mBorderSize == -1)
	{
		CMDIFrameWndEx::OnNcCalcSize(calcValidRects, lpncsp);
		mBorderSize = std::abs(rect.left - lpncsp->rgrc[0].left);
	}

	if (mMaximized)
	{
		lpncsp->rgrc[0].left = rect.left + mBorderSize;
		lpncsp->rgrc[0].top = rect.top + mBorderSize;
		lpncsp->rgrc[0].right = rect.right - mBorderSize;
		lpncsp->rgrc[0].bottom = rect.bottom - mBorderSize;
	}
	else
	{
		lpncsp->rgrc[0] = rect;
	}
}

LRESULT MainFrame::OnNcHitTest(CPoint point)
{
	CRect wndRect;
	GetWindowRect(&wndRect);
	CRect rect(wndRect);

	rect.right = rect.left + FRAME_BORDER_SIZE;
	rect.bottom = rect.top + FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOPLEFT;

	rect = wndRect;
	rect.left = rect.right - FRAME_BORDER_SIZE;
	rect.top = rect.bottom - FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOMRIGHT;

	rect = wndRect;
	rect.right = rect.left + FRAME_BORDER_SIZE;
	rect.top = rect.bottom - FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOMLEFT;

	rect = wndRect;
	rect.left = rect.right - FRAME_BORDER_SIZE;
	rect.bottom = rect.top + FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOPRIGHT;

	rect = wndRect;
	rect.bottom = rect.top + FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOP;

	rect = wndRect;
	rect.right = rect.left + FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTLEFT;

	rect = wndRect;
	rect.left = rect.right - FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTRIGHT;

	rect = wndRect;
	rect.top = rect.bottom - FRAME_BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOM;

	rect = wndRect;
	rect.right -= FRAME_BORDER_SIZE;
	rect.bottom = rect.top + CAPTION_HEIGHT;
	if (rect.PtInRect(point)) return HTCAPTION;

	return CMDIFrameWndEx::OnNcHitTest(point);
}

void MainFrame::OnNcLButtonDblClk(UINT hitTest, CPoint point)
{
	if (hitTest == HTCAPTION)
	{
		if (mMaximized)	{ PostMessageW(WM_SYSCOMMAND, SC_RESTORE); }
		else			{ PostMessageW(WM_SYSCOMMAND, SC_MAXIMIZE); }
	}
	else
	{
		CMDIFrameWndEx::OnNcLButtonDblClk(hitTest, point);
	}
}

BOOL MainFrame::OnNcActivate(BOOL active)
{
	Invalidate(FALSE);
	return CMDIFrameWndEx::OnNcActivate(active);
}

void MainFrame::OnWindowPosChanged(WINDOWPOS* windowPos)
{
	CMDIFrameWndEx::OnWindowPosChanged(windowPos);

	if (mShadowBorders[0].GetSafeHwnd())
	{
		HDWP hdwp = BeginDeferWindowPos(4);

		int x, y, cx, cy;

		x = windowPos->x - ShadowBorder::FIXED_SIZE;
		y = windowPos->y - ShadowBorder::FIXED_SIZE;
		cx = ShadowBorder::FIXED_SIZE;
		cy = windowPos->cy + 2 * ShadowBorder::FIXED_SIZE;
		DeferWindowPos(hdwp, mShadowBorders[0], m_hWnd, x, y, cx, cy, SWP_NOACTIVATE);

		x = windowPos->x + windowPos->cx;
		y = windowPos->y - ShadowBorder::FIXED_SIZE;
		cx = ShadowBorder::FIXED_SIZE;
		cy = windowPos->cy + 2 * ShadowBorder::FIXED_SIZE;
		DeferWindowPos(hdwp, mShadowBorders[1], m_hWnd, x, y, cx, cy, SWP_NOACTIVATE);

		x = windowPos->x;
		y = windowPos->y - ShadowBorder::FIXED_SIZE;
		cx = windowPos->cx;
		cy = ShadowBorder::FIXED_SIZE;
		DeferWindowPos(hdwp, mShadowBorders[2], m_hWnd, x, y, cx, cy, SWP_NOACTIVATE);

		x = windowPos->x;
		y = windowPos->y + windowPos->cy;
		cx = windowPos->cx;
		cy = ShadowBorder::FIXED_SIZE;
		DeferWindowPos(hdwp, mShadowBorders[3], m_hWnd, x, y, cx, cy, SWP_NOACTIVATE);

		EndDeferWindowPos(hdwp);
	}

	RedrawMdiTabAreas();
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	RECT workArea;
	SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
	int wWidth = workArea.right - workArea.left;
	int wHeight = workArea.bottom - workArea.top;
	cs.x = (wWidth - MIN_WND_WIDTH) / 2;
	cs.y = (wHeight - MIN_WND_HEIGHT) / 2;
	cs.cx = MIN_WND_WIDTH;
	cs.cy = MIN_WND_HEIGHT;

	cs.style =
		WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

	CMDIFrameWndEx::PreCreateWindow(cs);

	if (theApp.m_nCmdShow == SW_SHOWMAXIMIZED)
	{
		cs.style |= WS_MAXIMIZE;
	}

	return TRUE;
}

LRESULT CALLBACK MainFrame::ClientAreaWndProcEntry(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ((MainFrame*)theApp.m_pMainWnd)->ClientAreaWndProc(hWnd, msg, wParam, lParam);
}

LRESULT MainFrame::ClientAreaWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_ERASEBKGND)
	{
		return TRUE;
	}
	else if (msg == WM_PAINT)
	{
		CPaintDC dc(&m_wndClientArea);

		CRect rect;
		m_wndClientArea.GetClientRect(&rect);

		static const int BACKGROUND_IMAGE_WIDTH = 420;
		static const int BACKGROUND_IMAGE_HEIGHT = 88;

		D2D1_RECT_F imageRect;
		imageRect.left = static_cast<FLOAT>((rect.Width() - BACKGROUND_IMAGE_WIDTH) / 2);
		imageRect.right = imageRect.left + BACKGROUND_IMAGE_WIDTH;
		imageRect.top = static_cast<FLOAT>((rect.Height() - BACKGROUND_IMAGE_HEIGHT) / 2);
		imageRect.bottom = imageRect.top + BACKGROUND_IMAGE_HEIGHT;

		D2DRenderer::dcRenderTarget->BindDC(dc.m_hDC, &rect);
		D2DRenderer::dcRenderTarget->BeginDraw();
		D2DRenderer::dcRenderTarget->Clear(VisualManager::CLR_BASE);
		D2DRenderer::dcRenderTarget->DrawBitmap(mBackground, imageRect);
		D2DRenderer::dcRenderTarget->EndDraw();

		return 0;
	}
	else if (msg == AFX_WM_ON_TABGROUPMOUSEMOVE)
	{
		mMovingTab = true;
	}
	else if (msg == AFX_WM_GETDRAGBOUNDS)
	{
		LockWindowUpdate();
		mResizingTab = true;
	}
	else if (msg == WM_SIZE)
	{
		m_wndClientArea.Invalidate(FALSE);
	}

	return CallWindowProcW(mClientAreaSuperWndProc, hWnd, msg, wParam, lParam);
}

BOOL MainFrame::PreTranslateMessage(MSG* msg)
{
	if (msg->message == WM_LBUTTONDOWN ||
		msg->message == WM_RBUTTONDOWN ||
		msg->message == WM_LBUTTONUP ||
		msg->message == WM_MOUSEMOVE ||
		msg->message == WM_MOUSELEAVE)
	{
		CWnd* wnd = FromHandlePermanent(msg->hwnd);
		if (wnd)
		{
			if (&m_wndClientArea == wnd->GetParent())
			{
				if (wnd->IsKindOf(RUNTIME_CLASS(CMFCTabCtrl)))
				{
					if (msg->message == WM_MOUSEMOVE)
					{
						if (mTrackingTabMouseLeave == false)
						{
							TRACKMOUSEEVENT trackMouseEvent;
							trackMouseEvent.cbSize = sizeof(trackMouseEvent);
							trackMouseEvent.dwFlags = TME_LEAVE;
							trackMouseEvent.hwndTrack = msg->hwnd;
							TrackMouseEvent(&trackMouseEvent);

							mTrackingTabMouseLeave = true;
						}

						if (!mMovingTab && !mResizingTab)
						{
							wnd->Invalidate(FALSE);
						}
					}
					else if (msg->message == WM_MOUSELEAVE)
					{
						mTrackingTabMouseLeave = false;

						if (!mMovingTab && !mResizingTab)
						{
							wnd->Invalidate(FALSE);
							wnd->PostMessageW(WM_CANCELMODE);
						}
					}
					else if (msg->message == WM_LBUTTONUP)
					{
						if (mMovingTab || mResizingTab)
						{
							if (mResizingTab)
							{
								UnlockWindowUpdate();
							}
							mMovingTab = false;
							mResizingTab = false;
							SetTimer(UDATE_MDI_TAB_TIMER, 10, nullptr);
						}
						else
						{
							wnd->Invalidate(FALSE);
						}
					}
					else
					{
						if (!mMovingTab && !mResizingTab)
						{
							wnd->Invalidate(FALSE);
						}
					}
				}
			}
		}
	}

	return CMDIFrameWndEx::PreTranslateMessage(msg);
}

bool MainFrame::IsMdiActive() const
{
	return mMdiActive;
}

void MainFrame::RedrawMdiTabAreas()
{
	const CObList& tabGroups = m_wndClientArea.GetMDITabGroups();
	POSITION pos = tabGroups.GetHeadPosition();
	while (pos != nullptr)
	{
		CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabGroups.GetNext(pos));
		tabCtrl->Invalidate(FALSE);
	}
}

void MainFrame::OnTimer(UINT_PTR timerID)
{
	if (INITIAL_MODEL_OPEN_TIMER == timerID)
	{
		KillTimer(INITIAL_MODEL_OPEN_TIMER);

		LockWindowUpdate();
		const auto& models = theModelManager->GetModels();
		for (auto model : models)
		{
			ViewFrame* viewFrame = nullptr;
			for (auto cvPair : mViews)
			{
				if (cvPair.first == model)
				{
					viewFrame = cvPair.second;
					break;
				}
			}

			if (viewFrame)
			{
				MDIActivate(viewFrame);
			}
			else
			{
				LevelFrame* levelFrame = new LevelFrame(model, this);
				mViews.push_back({ model, levelFrame });
			}
		}
		UnlockWindowUpdate();
	}
	else if (DELAYED_REDRAW_TIMER == timerID)
	{
		KillTimer(DELAYED_REDRAW_TIMER);
		
		RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
	else if (UDATE_MDI_TAB_TIMER == timerID)
	{
		KillTimer(UDATE_MDI_TAB_TIMER);

		RedrawMdiTabAreas();
		RecalcLayout(FALSE);
	}
	else
	{
		CMDIFrameWndEx::OnTimer(timerID);
	}
}

bool MainFrame::IsFrameVisible(const ViewFrame* const viewFrame) const
{
	const CObList& tabGroups = m_wndClientArea.GetMDITabGroups();
	if (tabGroups.GetCount() > 0)
	{
		POSITION pos = tabGroups.GetHeadPosition();
		CMFCTabCtrl* tabCtrl;

		do
		{
			tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabGroups.GetNext(pos));
			int activeTab = tabCtrl->GetActiveTab();
			CWnd* frameWnd = tabCtrl->GetTabWndNoWrapper(activeTab);
			if (frameWnd == viewFrame)
			{
				return true;
			}
		}
		while (pos != nullptr);
	}

	return false;
}

bool MainFrame::UpdateMdiActivity()
{
	bool prevState = mMdiActive;

	mMdiActive = false;
	CWnd* focusWnd = GetFocus();
	if (focusWnd)
	{
		if (m_wndClientArea.m_hWnd == focusWnd->m_hWnd ||
			m_wndClientArea.IsChild(focusWnd))
		{
			mMdiActive = true;
		}
	}

	return (prevState != mMdiActive);
}

LRESULT MainFrame::OnIdleUpdateCmdUI(WPARAM wp, LPARAM lp)
{
	if (UpdateMdiActivity())
	{
		if (!mMovingTab && !mResizingTab)
		{
			RedrawMdiTabAreas();
		}
	}
	return CMDIFrameWndEx::OnIdleUpdateCmdUI(wp, lp);
}

int MainFrame::OnCreate(LPCREATESTRUCT createStruct)
{
	CMDIFrameWndEx::OnCreate(createStruct);

	EnableDocking(CBRS_ALIGN_ANY);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_tabLocation = CMFCBaseTabCtrl::LOCATION_TOP;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;
	mdiTabParams.m_bTabIcons = TRUE;
	mdiTabParams.m_bAutoColor = FALSE;
	mdiTabParams.m_bDocumentMenu = TRUE;
	mdiTabParams.m_bFlatFrame = TRUE;
	mdiTabParams.m_nTabBorderSize = 0;
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	for (int i = 0; i < ShadowBorder::NUMBER_OF_ROLES; ++i)
	{
		mShadowBorders[i].CreateWnd(this, static_cast<ShadowBorder::Role>(i));
	}

	mMinimizeButton.CreateWnd(this);
	mMaxRestoreButton.CreateWnd(this);
	mCloseButton.CreateWnd(this);

	mMenuStrip.Create(this);
	DWORD paneStyle = mMenuStrip.GetPaneStyle();
	paneStyle &= ~CBRS_GRIPPER;
	mMenuStrip.SetPaneStyle(paneStyle | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	DockPane(&mMenuStrip);

	mToolStrip.CreateEx(this, TBSTYLE_FLAT,
		WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	paneStyle = mToolStrip.GetPaneStyle();
	paneStyle &= ~CBRS_GRIPPER;
	mToolStrip.SetPaneStyle(paneStyle);

	CMFCToolBar::AddToolBarForImageCollection(HSMN_IDR_TOOLBAR_FOR_MENU, HSMN_IDR_TOOLBAR_FOR_MENU);

	mToolStrip.LoadToolBar(HSMN_IDR_MAIN_FRAME);
	DockPane(&mToolStrip);

	mInfoGadget.Create(nullptr, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		{ 0, 0, INFO_GADGET_SIZE, FIXED_STATUS_GADGET_HEIGHT }, this, AFX_IDW_STATUS_BAR);

	mProgressGadget.Create(nullptr, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		{ 0, 0, 8, FIXED_STATUS_GADGET_HEIGHT }, this, 0);

	mProjectPane.CreateWnd(this, WND_ID_PROJECT_PANE, CRect(0, 0, 200, 500));
	mProjectPane.SetWindowTextW(L"Projects");
	mProjectPane.SetPaneAlignment(CBRS_ALIGN_LEFT);
	mProjectPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&mProjectPane);

	mLibraryPane.CreateWnd(this, WND_ID_LIBRARY_PANE, CRect(0, 0, 200, 500));
	mLibraryPane.SetWindowTextW(L"Library");
	mLibraryPane.SetPaneAlignment(CBRS_ALIGN_LEFT);
	mLibraryPane.EnableDocking(CBRS_ALIGN_ANY);                                                                                                                                                                                                                                                                   
	DockPane(&mLibraryPane);

	mPropertyPane.CreateWnd(this, WND_ID_PROPERTY_PANE, CRect(0, 0, 200, 500));
	mPropertyPane.SetWindowTextW(L"Property");
	mPropertyPane.SetPaneAlignment(CBRS_ALIGN_LEFT);
	mPropertyPane.EnableDocking(CBRS_ALIGN_ANY);                                                                                                                                                                                                                                                                   
	DockPane(&mPropertyPane);

	mProjectPane.DockToWindow(&mPropertyPane, CBRS_TOP);
	mLibraryPane.AttachToTabWnd(&mProjectPane, DM_UNKNOWN);

	mLogPane.CreateWnd(this, WND_ID_LOG_PANE, CRect(0, 0, 100, 200));
	mLogPane.SetWindowTextW(L"Log");
	mLogPane.SetPaneAlignment(CBRS_ALIGN_BOTTOM);
	mLogPane.EnableDocking(CBRS_ALIGN_ANY);                                                                                                                                                                                                                                                                   
	DockPane(&mLogPane);

	mMonitoringPane.CreateWnd(this, WND_ID_MONITORING_PANE, CRect(0, 0, 200, 500));
	mMonitoringPane.SetWindowTextW(L"Monitoring");
	mMonitoringPane.SetPaneAlignment(CBRS_ALIGN_RIGHT);
	mMonitoringPane.EnableDocking(CBRS_ALIGN_ANY);                                                                                                                                                                                                                                                                   
	DockPane(&mMonitoringPane);
	
	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mBrush);

	WicBitmap wicBitmap(theApp.m_hInstance, HSMN_IDR_CAPTION_ICON_ACTIVE);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIconActive);
	
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_CAPTION_ICON_INACTIVE);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIconInactive);

	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_BACKGROUND);
	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mBackground);

	m_wndClientArea.ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);
	mClientBackgoundBrush.CreateSolidBrush(VisualManager::ToGdiColor(VisualManager::CLR_BASE));
	SetClassLongPtrW(m_wndClientArea, GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(mClientBackgoundBrush.m_hObject));
	mClientAreaSuperWndProc = reinterpret_cast<WNDPROC>(
		SetWindowLongPtrW(m_wndClientArea, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ClientAreaWndProcEntry)));

	theNotiCenter->RegisterListener(static_cast<INotiListener*>(this));

	return 0;
}

void MainFrame::OnDestroy()
{
	theNotiCenter->UnregisterListener(static_cast<INotiListener*>(this));

	SetWindowLongPtrW(m_wndClientArea, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(mClientAreaSuperWndProc));
	mBackground->Release();

	mBrush->Release();
	mIconActive->Release();
	mIconInactive->Release();
	mRenderer.Destroy();

	CMDIFrameWndEx::OnDestroy();
}

void MainFrame::OnSize(UINT nType, int cx, int cy)
{
	bool prevMaximizeState = mMaximized;
	if (nType == SIZE_MAXIMIZED)	mMaximized = true;
	else							mMaximized = false;
	if (prevMaximizeState != mMaximized)
	{
		UINT swpFlags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED;
		SetWindowPos(nullptr, 0, 0, 0, 0, swpFlags);
		return;
	}
	m_dockManager.m_rectInPlace.left = FRAME_BORDER_SIZE;
	m_dockManager.m_rectInPlace.right = cx - FRAME_BORDER_SIZE;
	m_dockManager.m_rectInPlace.top = FRAME_BORDER_SIZE + CAPTION_HEIGHT;
	m_dockManager.m_rectInPlace.bottom = cy - FRAME_BORDER_SIZE - STATUS_HEIGHT;
	CMDIFrameWndEx::OnSize(nType, cx, cy);

	mRenderer.Resize(cx, cy);

	if (mMinimizeButton.GetSafeHwnd())
	{
		int x = cx - SystemButton::FIXED_BUTTON_WIDTH - FRAME_BORDER_SIZE;
		mCloseButton.MoveWnd(x, FRAME_BORDER_SIZE);
		mCloseButton.Invalidate(FALSE);

		x -= SystemButton::FIXED_BUTTON_WIDTH;
		mMaxRestoreButton.MoveWnd(x, FRAME_BORDER_SIZE);
		mMaxRestoreButton.Invalidate(FALSE);

		x -= SystemButton::FIXED_BUTTON_WIDTH;
		mMinimizeButton.MoveWnd(x, FRAME_BORDER_SIZE);
		mMinimizeButton.Invalidate(FALSE);

		int statusGadgetY = cy - FRAME_BORDER_SIZE - FIXED_STATUS_GADGET_HEIGHT;

		mInfoGadget.SetWindowPos(nullptr,
			FRAME_BORDER_SIZE,
			statusGadgetY,
			0,
			0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

		mProgressGadget.SetWindowPos(nullptr,
			FRAME_BORDER_SIZE + INFO_GADGET_SIZE + 2,
			statusGadgetY,
			cx - FRAME_BORDER_SIZE - INFO_GADGET_SIZE - FRAME_BORDER_SIZE - 2,
			FIXED_STATUS_GADGET_HEIGHT,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	SetTimer(DELAYED_REDRAW_TIMER, 10, nullptr);
}

void MainFrame::AdjustDockingLayout(HDWP hdwp)
{
	LockWindowUpdate();

	CMDIFrameWndEx::AdjustDockingLayout(hdwp);
	
	UnlockWindowUpdate();
}

void MainFrame::OnClose()
{
	ShowWindow(SW_HIDE);

	CMDIFrameWndEx::OnClose();
}

void MainFrame::OnViewProjectPane()
{
	if (mProjectPane.IsPaneVisible())		mProjectPane.ShowPane(FALSE, FALSE, TRUE);
	else									mProjectPane.ShowPane(TRUE, FALSE, TRUE);
}

void MainFrame::OnViewLibraryPane()
{
	if (mLibraryPane.IsPaneVisible())		mLibraryPane.ShowPane(FALSE, FALSE, TRUE);
	else									mLibraryPane.ShowPane(TRUE, FALSE, TRUE);
}

void MainFrame::OnViewPropertyPane()
{
	if (mPropertyPane.IsPaneVisible())		mPropertyPane.ShowPane(FALSE, FALSE, TRUE);
	else									mPropertyPane.ShowPane(TRUE, FALSE, TRUE);
}

void MainFrame::OnViewLogPane()
{
	if (mLogPane.IsPaneVisible())			mLogPane.ShowPane(FALSE, FALSE, TRUE);
	else									mLogPane.ShowPane(TRUE, FALSE, TRUE);
}

void MainFrame::OnViewMonitoringPane()
{
	if (mMonitoringPane.IsPaneVisible())	mMonitoringPane.ShowPane(FALSE, FALSE, TRUE);
	else									mMonitoringPane.ShowPane(TRUE, FALSE, TRUE);
}

void MainFrame::OnViewProjectPaneUpdateCmdUI(CCmdUI* cmdUI)
{
	if (mProjectPane.IsPaneVisible())		cmdUI->SetCheck(1);
	else									cmdUI->SetCheck(0);
}

void MainFrame::OnViewLibraryPaneUpdateCmdUI(CCmdUI* cmdUI)
{
	if (mLibraryPane.IsPaneVisible())		cmdUI->SetCheck(1);
	else									cmdUI->SetCheck(0);
}

void MainFrame::OnViewPropertyPaneUpdateCmdUI(CCmdUI* cmdUI)
{
	if (mPropertyPane.IsPaneVisible())		cmdUI->SetCheck(1);
	else									cmdUI->SetCheck(0);
}

void MainFrame::OnViewLogPaneUpdateCmdUI(CCmdUI* cmdUI)
{
	if (mLogPane.IsPaneVisible())			cmdUI->SetCheck(1);
	else									cmdUI->SetCheck(0);
}

void MainFrame::OnViewMonitoringPaneUpdateCmdUI(CCmdUI* cmdUI)
{
	if (mMonitoringPane.IsPaneVisible())	cmdUI->SetCheck(1);
	else									cmdUI->SetCheck(0);
}

void MainFrame::OnProjectCompile()
{
	auto model = theModelManager->GetActiveModel();
	if (model)
	{
		if (nullptr == mCodeView)
		{
			mCodeView = new CodeView;
			mCodeView->CreateWnd(this);
		}
		mCodeView->Update(model);

		mCodeView->BringWindowToTop();
		mCodeView->ShowWindow(SW_SHOW);
	}
	else
	{
		LogItem logItem;
		logItem.senderID = Entity::NULL_ID;
		logItem.senderName = L"System";
		logItem.message = L"No active model to generate code for.";
		theLogMediator->TransferLogItem(std::move(logItem));
	}
}

void MainFrame::OnProjectDownload()
{
	auto model = theModelManager->GetActiveModel();
	if (model)
	{
		if (nullptr == mCodeView)
		{
			mCodeView = new CodeView;
			mCodeView->CreateWnd(this);
		}
		mCodeView->UpdateAndDownload(model);

		mCodeView->BringWindowToTop();
		mCodeView->ShowWindow(SW_SHOW);
	}
	else
	{
		LogItem logItem;
		logItem.senderID = Entity::NULL_ID;
		logItem.senderName = L"System";
		logItem.message = L"No active model to generate code for.";
		theLogMediator->TransferLogItem(std::move(logItem));
	}
}

void MainFrame::OnToolSettings()
{
	SettingsDialog dlg;
	dlg.DoModal();
}

void MainFrame::OnToolTagAuthor()
{
	if (nullptr == mTagAuthorView)
	{
		mTagAuthorView = new TagAuthorView;
		mTagAuthorView->CreateWnd(this);
	}
	mTagAuthorView->BringWindowToTop();
	mTagAuthorView->ShowWindow(SW_SHOW);
}

void MainFrame::OnInitMenuPopup(CMenu* popupMenu, UINT index, BOOL sysMenu)
{
	if (index == 6)
	{
		auto firstItem = popupMenu->GetMenuItemID(0);
		if (firstItem == HSMN_IDC_WINDOWS_CLOSE_ALL)
		{
			mCommandToView.clear();

			int numViews = static_cast<int>(mViews.size());
			int diffToMax = numViews - 10;
			list<ComponetViewPair>::const_reverse_iterator iter = mViews.rbegin();
			if (diffToMax > 0)
			{
				for (int i = 0; i < diffToMax; ++i)
				{
					iter++;
				}
			}

			UINT command = HSMN_IDC_WINDOWS_MANAGER;
			for (/**/; iter != mViews.rend(); ++iter)
			{
				wstring path(std::move(iter->second->GetPathText()));
				popupMenu->InsertMenuW(command, MF_BYCOMMAND, command - 1, path.c_str());
				
				command--;
				mCommandToView[command] = { iter->second, iter->second->GetNameText(), path };

				if (IsFrameVisible(iter->second))
				{
					popupMenu->CheckMenuItem(command, MF_CHECKED);
				}
			}
		}
	}
	CMDIFrameWndEx::OnInitMenuPopup(popupMenu, index, sysMenu);
}

void MainFrame::OnWindowCloseAll()
{
	list<ComponetViewPair> wndList(std::move(mViews));
	for (auto cvPair : wndList)
	{
		cvPair.second->DestroyWindow();
	}
}

void MainFrame::OnWindowManager()
{
	WindowDialog dlg;
	dlg.DoModal();
}

void MainFrame::OnActivateWindow(UINT cmd)
{
	if (HSMN_IDC_WINDOWS_CLOSE_ALL < cmd && cmd < HSMN_IDC_WINDOWS_MANAGER)
	{
		unordered_map<UINT, ViewFrameInfo>::const_iterator iter;
		iter = mCommandToView.find(cmd);
		if (iter != mCommandToView.end())
		{
			MDIActivate(iter->second.viewFrame);
		}
	}
}

void MainFrame::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	D2D1_RECT_F rect;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	rect.left = FRAME_BORDER_SIZE + 4.0f;
	rect.top = FRAME_BORDER_SIZE + 2.0f;
	rect.right = rect.left + 24;
	rect.bottom = rect.top + 24;
	
	if (GetForegroundWindow() == this)
	{
		mRenderer.DrawBitmap(mIconActive, &rect);
	}
	else
	{
		mRenderer.DrawBitmap(mIconInactive, &rect);
	}

	CRect minimizeButtonRect;
	mMinimizeButton.GetWindowRect(&minimizeButtonRect);
	ScreenToClient(&minimizeButtonRect);
	rect.left = rect.right + 7;
	rect.right = minimizeButtonRect.left - 3.0f;
	rect.bottom = rect.top + CAPTION_HEIGHT - 5;

	CString titleString;
	Model* activeModel = theModelManager->GetActiveModel();
	if (activeModel)
	{
		titleString += activeModel->GetName().c_str();
		titleString += L"   ";
		titleString += activeModel->GetFilePath().c_str();
		titleString += L" - HRI Studio";
	}
	else
	{
		titleString = L"HRI Studio";
	}

	if (GetForegroundWindow() == this)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL1);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL2);
	}
	mRenderer.DrawText(titleString, titleString.GetLength(), DWrite::segoe12B, rect, mBrush);

	if (GetForegroundWindow() == this)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL2);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL3);
	}
	rect.left = 1;
	rect.top = 1;
	rect.right = (FLOAT)clientRect.right;
	rect.bottom = (FLOAT)clientRect.bottom;
	mRenderer.DrawRectangle(rect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
