#pragma once

#include <hsuiShadowBorder.h>
#include <hsuiSystemButtons.h>
#include "hsmnMenuStrip.h"
#include "hsmnToolStrip.h"
#include "hsmnProjectPane.h"
#include "hsmnLibraryPane.h"
#include "hsmnPropertyPane.h"
#include "hsmnLogPane.h"
#include "hsmnMonitoringPane.h"
#include "hsmnInfoGadget.h"
#include "hsmnProgressGadget.h"
#include "hsmnViewFrame.h"

namespace hsmn {

class LevelView;
class CodeView;
class TagAuthorView;

class MainFrame
	:
	public CMDIFrameWndEx,
	public xsc::INotiListener
{
	DECLARE_DYNAMIC(MainFrame)
	DECLARE_MESSAGE_MAP()

public:
	MainFrame();
	virtual ~MainFrame();

	virtual BOOL LoadFrame(UINT resourceID,
		DWORD defaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
		CWnd* parentWnd = nullptr, CCreateContext* context = nullptr) override;

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual BOOL PreTranslateMessage(MSG* msg) override;
	virtual void AdjustDockingLayout(HDWP hdwp = nullptr) override;

	virtual void OnNotiChildAddition(xsc::Component*, const xsc::ComponentSet&) override;
	virtual void OnNotiChildRemoval(xsc::Component*, const xsc::ComponentSet&) override;
	virtual void OnNotiPropertyChange(xsc::Component*, xsc::Entity::PropID) override;

	void OpenLevelView(xsc::Level* level);
	void EraseLevelView(xsc::Level* level);
	bool IsRenderable(LevelView* levelView) const;

	afx_msg void OnViewProjectPane();
	afx_msg void OnViewLibraryPane();
	afx_msg void OnViewPropertyPane();
	afx_msg void OnViewLogPane();
	afx_msg void OnViewMonitoringPane();

	afx_msg void OnProjectCompile();
	afx_msg void OnProjectDownload();
	
	afx_msg void OnToolSettings();
	afx_msg void OnToolTagAuthor();

	void RedrawMdiTabAreas();
	bool IsMdiActive() const;

protected:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* mmi);
	afx_msg void OnNcCalcSize(BOOL calcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT hitTest, CPoint point);
	afx_msg BOOL OnNcActivate(BOOL active);
	afx_msg void OnWindowPosChanged(WINDOWPOS* windowPos);

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

	afx_msg void OnSize(UINT type, int cx, int cy);
	
	afx_msg void OnViewProjectPaneUpdateCmdUI(CCmdUI* cmdUI);
	afx_msg void OnViewLibraryPaneUpdateCmdUI(CCmdUI* cmdUI);
	afx_msg void OnViewPropertyPaneUpdateCmdUI(CCmdUI* cmdUI);
	afx_msg void OnViewLogPaneUpdateCmdUI(CCmdUI* cmdUI);
	afx_msg void OnViewMonitoringPaneUpdateCmdUI(CCmdUI* cmdUI);

	afx_msg void OnActivateWindow(UINT cmd);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnWindowManager();
	afx_msg void OnInitMenuPopup(CMenu* popupMenu, UINT index, BOOL sysMenu);

	afx_msg void OnTimer(UINT_PTR timerID);
	afx_msg void OnPaint();

	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM, LPARAM);

	bool UpdateMdiActivity();
	bool IsFrameVisible(const ViewFrame* const viewFrame) const;

protected:
	static LRESULT CALLBACK ClientAreaWndProcEntry(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT ClientAreaWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	WNDPROC mClientAreaSuperWndProc;

	bool mMaximized;
	int mBorderSize;

	CBrush mClientBackgoundBrush;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap* mIconActive;
	ID2D1Bitmap* mIconInactive;
	ID2D1Bitmap* mBackground;

	hsui::ShadowBorder mShadowBorders[hsui::ShadowBorder::NUMBER_OF_ROLES];
	hsui::SystemButtonMinimize mMinimizeButton;
	hsui::SystemButtonMaxRestore mMaxRestoreButton;
	hsui::SystemButtonClose	mCloseButton;
	
	MenuStrip mMenuStrip;
	ToolStrip mToolStrip;
	
	InfoGadget mInfoGadget;
	ProgressGadget mProgressGadget;

	ProjectPane mProjectPane;
	LibraryPane	mLibraryPane;
	PropertyPane mPropertyPane;
	LogPane mLogPane;
	MonitoringPane mMonitoringPane;

	CodeView* mCodeView;
	TagAuthorView* mTagAuthorView;

	typedef std::pair<xsc::Component*, ViewFrame*> ComponetViewPair;
	std::list<ComponetViewPair> mViews;

	struct ViewFrameInfo
	{
		ViewFrame* viewFrame;
		std::wstring name;
		std::wstring path;
	};
	std::unordered_map<UINT, ViewFrameInfo> mCommandToView;

	bool mMdiActive;
	bool mMovingTab;
	bool mResizingTab;
	bool mTrackingTabMouseLeave;

	std::vector<ViewFrame*> mTempViewFrameVec;
	friend class WindowDialog;
};

} // namespace hsmn
