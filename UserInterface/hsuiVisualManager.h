#pragma once

#include <hsuiIApplication.h>
#include <afxvisualmanagerwindows7.h>

namespace hsui {

class HSUI_API VisualManager : public CMFCVisualManagerWindows7
{
	DECLARE_DYNCREATE(VisualManager)

public:
	static const D2D_COLOR_F CLR_BASE;
	static const D2D_COLOR_F CLR_DARK_BASE;

	static const D2D_COLOR_F CLR_LEVEL1;
	static const D2D_COLOR_F CLR_LEVEL2;
	static const D2D_COLOR_F CLR_LEVEL3;
	
	static const D2D_COLOR_F CLR_BLACK;
	static const D2D_COLOR_F CLR_WHITE;
	static const D2D_COLOR_F CLR_SEMI_WHITE;
	
	static const D2D_COLOR_F CLR_GREEN;
	static const D2D_COLOR_F CLR_ORANGE;

	static const D2D_COLOR_F CLR_BLUE;
	static const D2D_COLOR_F CLR_DARK_BLUE;
	
protected:
	VisualManager();
	virtual ~VisualManager();

public:
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* ctrl, CDC* dc,
		CRect& rect, BOOL isPressed, BOOL isHighlighted) override;

	virtual COLORREF GetToolbarButtonTextColor(
		CMFCToolBarButton* button, CMFCVisualManager::AFX_BUTTON_STATE state) override;
	
	virtual void OnFillBarBackground(
		CDC* dc, CBasePane* bar, CRect rectClient, CRect rectClip, BOOL ncArea) override;
	
	virtual void OnFillButtonInterior(
		CDC* dc, CMFCToolBarButton* button, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state) override;
	
	virtual COLORREF GetMenuItemTextColor(
		CMFCToolBarMenuButton* button, BOOL highlighted, BOOL disabled) override;

	virtual void OnHighlightMenuItem(
		CDC* dc, CMFCToolBarMenuButton* button, CRect rect, COLORREF& textColor) override;
	
	virtual void OnDrawMenuCheck(
		CDC* dc, CMFCToolBarMenuButton* button, CRect rect, BOOL highlight, BOOL isRadio) override;

	virtual void OnDrawMenuBorder(CDC* dc, CMFCPopupMenu* menu, CRect rect) override;

	virtual void OnDrawSeparator(CDC* dc, CBasePane* bar, CRect rect, BOOL isHorz) override;

	virtual COLORREF OnDrawPaneCaption(
		CDC* dc, CDockablePane* bar, BOOL active, CRect rectCaption, CRect rectButtons) override;

	virtual void OnDrawCaptionButton(CDC* dc, CMFCCaptionButton* button,
		BOOL active, BOOL horz, BOOL maximized, BOOL disabled, int imageID = -1) override;

	virtual void OnDrawPaneBorder(CDC* dc, CBasePane* bar, CRect& rect) override;
	
	virtual void OnDrawPaneDivider(CDC* dc, CPaneDivider* slider, CRect rect, BOOL autoHideMode) override;

	virtual COLORREF OnFillMiniFrameCaption(CDC* dc, CRect rectCaption, CPaneFrameWnd* frameWnd, BOOL active) override;

	virtual void OnDrawMiniFrameBorder(
		CDC* dc, CPaneFrameWnd* frameWnd, CRect rectBorder, CRect rectBorderSize) override;

	virtual int GetDockingTabsBordersSize() override;

	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* tabWnd,
		COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight,
		CBrush*& faceBrush, CBrush*& blackBrush) override;

	virtual void OnEraseTabsArea(CDC* dc, CRect rect, const CMFCBaseTabCtrl* tabWnd) override;

	virtual void OnDrawTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd) override;

	virtual void OnDrawTabResizeBar(
		CDC* dc, CMFCBaseTabCtrl* tabWnd, BOOL isVert, CRect rect, CBrush* faceBrush, CPen* pen) override;

	virtual AFX_SMARTDOCK_THEME GetSmartDockingTheme() override;

protected:
	void HandleMdiTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd);
	void HandleDockingTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd);

public:
	static inline COLORREF ToGdiColor(const D2D_COLOR_F& color);

	inline HINSTANCE GetAppInstanceHandle() const;
	inline const CString& GetAppTitle() const;
	inline UINT GetAppIconActive() const;
	inline UINT GetAppIconInactive() const;

	inline CFont* GetCommonFontR() const;
	inline CFont* GetCommonFontB() const;

	inline CBrush* GetBaseBrush() const;
	inline CBrush* GetDarkBaseBrush() const;
	inline CBrush* GetLevel1Brush() const;
	inline CBrush* GetLevel2Brush() const;
	inline CBrush* GetLevel3Brush() const;
	inline CBrush* GetWhiteBrush() const;
	inline CBrush* GetSemiWhiteBrush() const;
	inline CBrush* GetBlackBrush() const;
	inline CBrush* GetGreenBrush() const;
	inline CBrush* GetOrangeBrush() const;
	inline CBrush* GetBlueBrush() const;
	inline CBrush* GetDarkBlueBrush() const;
	
	inline CPen* GetBasePen() const;
	inline CPen* GetDarkBasePen() const;
	inline CPen* GetLevel1Pen() const;
	inline CPen* GetLevel2Pen() const;
	inline CPen* GetLevel3Pen() const;
	inline CPen* GetWhitePen() const;
	inline CPen* GetSemiWhitePen() const;
	inline CPen* GetBlackPen() const;
	inline CPen* GetGreenPen() const;
	inline CPen* GetOrangePen() const;
	inline CPen* GetBluePen() const;
	inline CPen* GetDarkBluePen() const;

protected:
	IApplication* mVisualDataProvider;
	HINSTANCE mAppInstanceHandle;
	CString	mAppTitle;
	UINT mAppIconActive;
	UINT mAppIconInactive;

	CImageList mMenuChecks;
	CImageList mMiniCloseButtons;

	std::unique_ptr<CFont> mCommonFontR;
	std::unique_ptr<CFont> mCommonFontB;

	std::unique_ptr<CBrush>	mBaseBrush;
	std::unique_ptr<CBrush>	mDarkBaseBrush;
	std::unique_ptr<CBrush> mLevel1Brush;
	std::unique_ptr<CBrush> mLevel2Brush;
	std::unique_ptr<CBrush> mLevel3Brush;
	std::unique_ptr<CBrush> mWhiteBrush;
	std::unique_ptr<CBrush> mSemiWhiteBrush;
	std::unique_ptr<CBrush> mBlackBrush;
	std::unique_ptr<CBrush> mGreenBrush;
	std::unique_ptr<CBrush> mOrangeBrush;
	std::unique_ptr<CBrush> mBlueBrush;
	std::unique_ptr<CBrush> mDarkBlueBrush;

	std::unique_ptr<CPen> mBasePen;
	std::unique_ptr<CPen> mDarkBasePen;
	std::unique_ptr<CPen> mLevel1Pen;
	std::unique_ptr<CPen> mLevel2Pen;
	std::unique_ptr<CPen> mLevel3Pen;
	std::unique_ptr<CPen> mWhitePen;
	std::unique_ptr<CPen> mSemiWhitePen;
	std::unique_ptr<CPen> mBlackPen;
	std::unique_ptr<CPen> mGreenPen;
	std::unique_ptr<CPen> mOrangePen;
	std::unique_ptr<CPen> mBluePen;
	std::unique_ptr<CPen> mDarkBluePen;

	CRect mDockablePaneCaptionRect;
	CRect mMultiFrameWndCaptionRect;

	// ================================================================================================================
	private: friend Module;
	static void InitVisualManager();
	static void TermVisualManager();
	// ================================================================================================================
};

inline COLORREF VisualManager::ToGdiColor(const D2D_COLOR_F& color)
{
	return RGB(std::ceil(color.r * 255), std::ceil(color.g * 255), std::ceil(color.b * 255));
}

inline HINSTANCE VisualManager::GetAppInstanceHandle() const
{
	return mAppInstanceHandle;
}

inline const CString& VisualManager::GetAppTitle() const
{
	return mAppTitle;
}

inline UINT VisualManager::GetAppIconActive() const
{
	return mAppIconActive;
}

inline UINT VisualManager::GetAppIconInactive() const
{
	return mAppIconInactive;
}

inline CFont* VisualManager::GetCommonFontR() const
{
	return mCommonFontR.get();
}

inline CFont* VisualManager::GetCommonFontB() const
{
	return mCommonFontB.get();
}

inline CBrush* VisualManager::GetBaseBrush() const
{
	return mBaseBrush.get();
}

inline CBrush* VisualManager::GetDarkBaseBrush() const
{
	return mDarkBaseBrush.get();
}

inline CBrush* VisualManager::GetLevel1Brush() const
{
	return mLevel1Brush.get();
}

inline CBrush* VisualManager::GetLevel2Brush() const
{
	return mLevel2Brush.get();
}

inline CBrush* VisualManager::GetLevel3Brush() const
{
	return mLevel3Brush.get();
}

inline CBrush* VisualManager::GetBlueBrush() const
{
	return mBlueBrush.get();
}

inline CBrush* VisualManager::GetDarkBlueBrush() const
{
	return mDarkBlueBrush.get();
}

inline CBrush* VisualManager::GetWhiteBrush() const
{
	return mWhiteBrush.get();
}

inline CBrush* VisualManager::GetSemiWhiteBrush() const
{
	return mSemiWhiteBrush.get();
}

inline CBrush* VisualManager::GetBlackBrush() const
{
	return mBlackBrush.get();
}

inline CBrush* VisualManager::GetGreenBrush() const
{
	return mGreenBrush.get();
}

inline CBrush* VisualManager::GetOrangeBrush() const
{
	return mOrangeBrush.get();
}

inline CPen* VisualManager::GetBasePen() const
{
	return mBasePen.get();
}

inline CPen* VisualManager::GetDarkBasePen() const
{
	return mDarkBasePen.get();
}

inline CPen* VisualManager::GetLevel1Pen() const
{
	return mLevel1Pen.get();
}

inline CPen* VisualManager::GetLevel2Pen() const
{
	return mLevel2Pen.get();
}

inline CPen* VisualManager::GetLevel3Pen() const
{
	return mLevel3Pen.get();
}

inline CPen* VisualManager::GetWhitePen() const
{
	return mWhitePen.get();
}

inline CPen* VisualManager::GetSemiWhitePen() const
{
	return mSemiWhitePen.get();
}

inline CPen* VisualManager::GetBlackPen() const
{
	return mBlackPen.get();
}

inline CPen* VisualManager::GetGreenPen() const
{
	return mGreenPen.get();
}

inline CPen* VisualManager::GetOrangePen() const
{
	return mOrangePen.get();
}

inline CPen* VisualManager::GetBluePen() const
{
	return mBluePen.get();
}

inline CPen* VisualManager::GetDarkBluePen() const
{
	return mDarkBluePen.get();
}

extern HSUI_API VisualManager* theVisualManager;

} // namespace hsui
