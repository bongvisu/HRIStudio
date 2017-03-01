#include "hsuiPrecompiled.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include "hsuiDockingTabCtrl.h"
#include <afxcustomizebutton.h>

using namespace std;
using namespace xsc;

namespace hsui {

// ====================================================================================================================
VisualManager* theVisualManager = nullptr;

void VisualManager::InitVisualManager()
{
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(VisualManager));
	CMFCVisualManager::GetInstance();
}

void VisualManager::TermVisualManager()
{
	CMFCVisualManager::DestroyInstance();
}
// ====================================================================================================================

static const int MINI_CLOSE_BUTTON_SIZE = 16;
static const unsigned int MINI_CLOSE_BUTTON_DARK = 0;
static const unsigned int MINI_CLOSE_BUTTON_GRAY = 1;
static const unsigned int MINI_CLOSE_BUTTON_LIGHT_GRAY = 2;
static const unsigned int MINI_CLOSE_BUTTON_SEMI_WHITE = 3;
static const unsigned int MINI_CLOSE_BUTTON_WHITE = 4;

static const int MENU_CHECK_SIZE = 24;
static const unsigned int MENU_CHECK_E = 0;
static const unsigned int MENU_CHECK_D = 1;

const D2D_COLOR_F VisualManager::CLR_BASE = { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_DARK_BASE = { 221 / 255.0f, 221 / 255.0f, 230 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_LEVEL3 = { 204 / 255.0f, 206 / 255.0f, 219 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_LEVEL2 = { 135 / 255.0f, 140 / 255.0f, 145 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_LEVEL1 = { 50 / 255.0f,  55 / 255.0f, 60 / 255.0f, 1.0f };

const D2D_COLOR_F VisualManager::CLR_WHITE = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_SEMI_WHITE = { 250 / 255.0f, 250 / 255.0f, 252 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_BLACK = { 0 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f };

const D2D_COLOR_F VisualManager::CLR_GREEN = { 88 / 255.0f, 170 / 255.0f, 137 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_ORANGE = { 255 / 255.0f, 165 / 255.0f, 0 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_BLUE = { 51 / 255.0f, 153 / 255.0f, 255 / 255.0f, 1.0f };
const D2D_COLOR_F VisualManager::CLR_DARK_BLUE = { 0 / 255.0f, 130 / 255.0f, 230 / 255.0f, 1.0f };

IMPLEMENT_DYNCREATE(VisualManager, CMFCVisualManagerWindows7)

VisualManager::VisualManager()
{
	// ----------------------------------------------------------------------------------------------------------------

	mCommonFontR = make_unique<CFont>();
	mCommonFontR->CreateFontW(
		16,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
		L"Segoe UI");				// lpszFacename

	mCommonFontB = make_unique<CFont>();
	mCommonFontB->CreateFontW(
		16,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_BOLD,					// nWeight
		FALSE,						// bItalic
		FALSE,						// bUnderline
		0,							// cStrikeOut
		ANSI_CHARSET,				// nCharSet
		OUT_DEFAULT_PRECIS,			// nOutPrecision
		CLIP_DEFAULT_PRECIS,		// nClipPrecision
		DEFAULT_QUALITY,			// nQuality
		DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
		L"Segoe UI");				// lpszFacename

	mBaseBrush = make_unique<CBrush>(ToGdiColor(CLR_BASE));
	mDarkBaseBrush = make_unique<CBrush>(ToGdiColor(CLR_DARK_BASE));
	mLevel1Brush = make_unique<CBrush>(ToGdiColor(CLR_LEVEL1));
	mLevel2Brush = make_unique<CBrush>(ToGdiColor(CLR_LEVEL2));
	mLevel3Brush = make_unique<CBrush>(ToGdiColor(CLR_LEVEL3));
	mWhiteBrush = make_unique<CBrush>(ToGdiColor(CLR_WHITE));
	mSemiWhiteBrush = make_unique<CBrush>(ToGdiColor(CLR_SEMI_WHITE));
	mBlackBrush = make_unique<CBrush>(ToGdiColor(CLR_BLACK));
	mGreenBrush = make_unique<CBrush>(ToGdiColor(CLR_GREEN));
	mOrangeBrush = make_unique<CBrush>(ToGdiColor(CLR_ORANGE));
	mBlueBrush = make_unique<CBrush>(ToGdiColor(CLR_BLUE));
	mDarkBlueBrush = make_unique<CBrush>(ToGdiColor(CLR_DARK_BLUE));

	mBasePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_BASE));
	mDarkBasePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_DARK_BASE));
	mLevel1Pen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_LEVEL1));
	mLevel2Pen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_LEVEL2));
	mLevel3Pen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_LEVEL3));
	mWhitePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_WHITE));
	mSemiWhitePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_SEMI_WHITE));
	mBlackPen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_BLACK));
	mGreenPen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_GREEN));
	mOrangePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_ORANGE));
	mBluePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_BLUE));
	mDarkBluePen = make_unique<CPen>(PS_SOLID, 1, ToGdiColor(CLR_DARK_BLUE));

	mMiniCloseButtons.Create(MINI_CLOSE_BUTTON_SIZE, MINI_CLOSE_BUTTON_SIZE, ILC_COLOR32, 5, 0);
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MINI_CLOSE_BUTTON_DARK, hsui::Module::handle);
		mMiniCloseButtons.Add(&iconImage, RGB(0, 0, 0));
	}
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MINI_CLOSE_BUTTON_GRAY, hsui::Module::handle);
		mMiniCloseButtons.Add(&iconImage, RGB(0, 0, 0));
	}
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MINI_CLOSE_BUTTON_LIGHT_GRAY, hsui::Module::handle);
		mMiniCloseButtons.Add(&iconImage, RGB(0, 0, 0));
	}
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MINI_CLOSE_BUTTON_SEMI_WHITE, hsui::Module::handle);
		mMiniCloseButtons.Add(&iconImage, RGB(0, 0, 0));
	}
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MINI_CLOSE_BUTTON_WHITE, hsui::Module::handle);
		mMiniCloseButtons.Add(&iconImage, RGB(0, 0, 0));
	}

	mMenuChecks.Create(MENU_CHECK_SIZE, MENU_CHECK_SIZE, ILC_COLOR32, 2, 0);
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MENU_CHECK_E, hsui::Module::handle);
		mMenuChecks.Add(&iconImage, RGB(0, 0, 0));
	}
	{
		CPngImage iconImage;
		iconImage.Load(HSUI_IDR_MENU_CHECK_D, hsui::Module::handle);
		mMenuChecks.Add(&iconImage, RGB(0, 0, 0));
	}

	// ----------------------------------------------------------------------------------------------------------------

	mVisualDataProvider = dynamic_cast<IApplication*>(AfxGetApp());
	mAppInstanceHandle = mVisualDataProvider->GetApplicationInstanceHandle();
	mAppTitle = mVisualDataProvider->GetApplicationTitle();
	mAppIconActive = mVisualDataProvider->GetApplicationIconActive();
	mAppIconInactive = mVisualDataProvider->GetApplicationIconInactive();

	CDockingManager::SetDockingMode(DT_SMART);
	CSmartDockingInfo& defaultInfo = CDockingManager::GetSmartDockingParams();
	CSmartDockingInfo newInfo;
	defaultInfo.CopyTo(newInfo);
	newInfo.m_bIsAlphaMarkers = TRUE;
	newInfo.m_uiMarkerBmpResID[0] = HSUI_IDR_DOCK_GUIDE_L;
	newInfo.m_uiMarkerBmpResID[1] = HSUI_IDR_DOCK_GUIDE_R;
	newInfo.m_uiMarkerBmpResID[2] = HSUI_IDR_DOCK_GUIDE_T;
	newInfo.m_uiMarkerBmpResID[3] = HSUI_IDR_DOCK_GUIDE_B;
	newInfo.m_uiMarkerBmpResID[4] = HSUI_IDR_DOCK_GUIDE_M;
	newInfo.m_uiBaseBmpResID = HSUI_IDR_DOCK_GUIDE_BASE;
	CDockingManager::SetSmartDockingParams(newInfo);

	LOGFONT logFont;
	mCommonFontR->GetLogFont(&logFont);
	CMFCMenuBar::SetMenuFont(&logFont);
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	// ----------------------------------------------------------------------------------------------------------------

	theVisualManager = this;
}

VisualManager::~VisualManager()
{
	theVisualManager = nullptr;
}

void VisualManager::OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* ctrl,
	CDC* dc, CRect& rect, BOOL isPressed, BOOL isHighlighted)
{
	// Intentionally do nothing.
}

COLORREF VisualManager::GetToolbarButtonTextColor(CMFCToolBarButton* button, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	BOOL disabled = (CMFCToolBar::IsCustomizeMode() && !button->IsEditable()) ||
		(!CMFCToolBar::IsCustomizeMode() &&(button->m_nStyle & TBBS_DISABLED));

	if (button->IsKindOf(RUNTIME_CLASS(CMFCOutlookBarPaneButton)))
	{
		if (GetGlobalData()->IsHighContrastMode())
		{
			return disabled ? GetGlobalData()->clrGrayedText : GetGlobalData()->clrWindowText;
		}

		return disabled ? GetGlobalData()->clrBtnFace : GetGlobalData()->clrWindow;
	}
	else if (button->IsKindOf(RUNTIME_CLASS(CMFCToolBarMenuButton)))
	{
		return ToGdiColor(CLR_LEVEL1);
	}

	if (disabled)
	{
		return GetGlobalData()->clrGrayedText;
	}
	else if (state == ButtonsIsHighlighted)
	{
		return CMFCToolBar::GetHotTextColor();
	}
	else
	{
		return GetGlobalData()->clrBarText;
	}
}

void VisualManager::OnFillBarBackground(CDC* dc, CBasePane* bar, CRect rectClient, CRect rectClip, BOOL ncArea)
{
	if (bar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
	{
		dc->FillRect(&rectClient, mSemiWhiteBrush.get());
	}
	else
	{
		dc->FillRect(&rectClient, mBaseBrush.get());
	}
}
	
void VisualManager::OnFillButtonInterior(
	CDC* dc, CMFCToolBarButton* button, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnFillButtonInterior(dc, button, rect, state);
		return; /* done */
	}

	CMFCCustomizeButton* custButton = DYNAMIC_DOWNCAST(CMFCCustomizeButton, button);
	if (custButton == nullptr)
	{
		if (CMFCToolBar::IsCustomizeMode() && !CMFCToolBar::IsAltCustomizeMode() && !button->IsLocked())
		{
			return; /* done */
		}

		int index = 0;
		CMFCControlRenderer* controlRenderer = nullptr;
		
		BOOL disabled = (button->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
		BOOL checked = (button->m_nStyle & TBBS_CHECKED ) == TBBS_CHECKED;

		CBasePane* bar = DYNAMIC_DOWNCAST(CBasePane, button->GetParentWnd());
		CMFCToolBarMenuButton* toolbarMenuButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, button);

		if (toolbarMenuButton != nullptr && bar != nullptr)
		{
			if (bar->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)) == FALSE &&
				bar->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar)))
			{
				if (checked)
				{
					return;
				}
				else if (state == ButtonsIsPressed || state == ButtonsIsHighlighted)
				{
					controlRenderer = &m_ctrlMenuHighlighted[disabled ? 1 : 0];
				}
				else
				{
					return;
				}
			}
		}

		if (controlRenderer != nullptr)
		{
			controlRenderer->Draw(dc, rect, index);
			return;
		}
	}

	BOOL isMenuBar = FALSE;
	BOOL isPopupMenu = FALSE;

	CMFCToolBarMenuButton* menuButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, button);
	if (menuButton != nullptr)
	{
		isMenuBar =
			menuButton->GetParentWnd() != nullptr &&
			menuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCMenuBar));
		isPopupMenu =
			menuButton->GetParentWnd() != nullptr &&
			menuButton->GetParentWnd()->IsKindOf(RUNTIME_CLASS(CMFCPopupMenuBar));
	}

	if (isPopupMenu && state != ButtonsIsHighlighted && state != ButtonsIsPressed)
	{
		return; 
	}

	int tsState = TS_NORMAL;

	if (button->m_nStyle & TBBS_DISABLED)
	{
		tsState = TS_DISABLED;
	}
	else if ((button->m_nStyle & TBBS_PRESSED) && state == ButtonsIsHighlighted)
	{
		tsState = TS_PRESSED;
	}
	else if (button->m_nStyle & TBBS_CHECKED)
	{
		tsState = (state == ButtonsIsHighlighted) ? TS_HOTCHECKED : TS_CHECKED;
	}
	else if (state == ButtonsIsHighlighted)
	{
		tsState = TS_HOT;

		if (menuButton != nullptr && menuButton->IsDroppedDown())
		{
			tsState = TS_PRESSED;
			rect.bottom--;
		}
	}

	if (isMenuBar && (tsState == TS_HOT || tsState == TS_PRESSED))
	{
		if (tsState == TS_HOT)
		{
			dc->FillRect(&rect, mLevel3Brush.get());
		}
		else
		{
			dc->FillRect(&rect, mSemiWhiteBrush.get());
			rect.bottom += 2;
			auto pressedBorderColor = ToGdiColor(CLR_LEVEL3);
			dc->Draw3dRect(&rect, pressedBorderColor, pressedBorderColor);
		}
	}
	else
	{
		DrawThemeBackground(m_hThemeToolBar, dc->GetSafeHdc(), TP_BUTTON, tsState, &rect, 0);
	}
}

COLORREF VisualManager::GetMenuItemTextColor(CMFCToolBarMenuButton* button, BOOL highlighted, BOOL disabled)
{
	if (disabled)
	{
		return ToGdiColor(CLR_LEVEL2);
	}
	else if (highlighted == FALSE)
	{
		return ToGdiColor(CLR_LEVEL1);
	}

	return CMFCVisualManagerWindows7::GetMenuItemTextColor(button, highlighted, disabled);
}

void VisualManager::OnDrawSeparator(CDC* dc, CBasePane* bar, CRect rect, BOOL isHorz)
{
	if (m_hThemeToolBar == nullptr || bar->IsDialogControl())
	{
		CMFCVisualManagerOfficeXP::OnDrawSeparator(dc, bar, rect, isHorz);
		return;
	}

	CMFCPopupMenuBar* menuBar = DYNAMIC_DOWNCAST(CMFCPopupMenuBar, bar);
	if (menuBar != nullptr)
	{
		if (m_bOfficeStyleMenus)
		{
			CMFCVisualManagerOfficeXP::OnDrawSeparator(dc, bar, rect, isHorz);
			return;
		}

		if (m_hThemeMenu != nullptr)
		{
			CMFCRibbonPanelMenuBar* ribbonMenuBar = DYNAMIC_DOWNCAST(CMFCRibbonPanelMenuBar, bar);
			BOOL isRibbonPanel =
				ribbonMenuBar != nullptr &&
				!ribbonMenuBar->IsMenuMode() &&
				!ribbonMenuBar->IsMainPanel();

			if (isRibbonPanel)
			{
				if (isHorz && rect.Width() < 2)
				{
					rect.right++;
				}

				DrawThemeBackground(m_hThemeToolBar, dc->GetSafeHdc(),
					isHorz ? TP_SEPARATOR : TP_SEPARATORVERT, 0, &rect, 0);
				return;
			}

			if (ribbonMenuBar == nullptr || !isRibbonPanel)
			{
				CRect rectClient;
				bar->GetClientRect(rectClient);

				int l = rectClient.left + menuBar->GetGutterWidth() + 4;
				int r = rectClient.right;
				int y = rect.top + 3;

				CPen* originalPen = dc->SelectObject(mLevel3Pen.get());

				dc->MoveTo(l, y);
				dc->LineTo(r, y);
				
				dc->SelectObject(originalPen);
			}
			else
			{
				DrawThemeBackground(m_hThemeMenu, dc->GetSafeHdc(), MENU_POPUPSEPARATOR, 0, &rect, 0);
			}
			return;
		}
	}

	if (isHorz && rect.Width() < 2)
	{
		rect.right++;
	}

	DrawThemeBackground(m_hThemeToolBar, dc->GetSafeHdc(), isHorz ? TP_SEPARATOR : TP_SEPARATORVERT, 0, &rect, 0);
}

void VisualManager::OnHighlightMenuItem(CDC* dc, CMFCToolBarMenuButton* button, CRect rect, COLORREF& clrText)
{
	if (!CanDrawImage())
	{
		CMFCVisualManagerWindows::OnHighlightMenuItem(dc, button, rect, clrText);
		return;
	}

	clrText = ToGdiColor(CLR_WHITE);
	dc->FillRect(&rect, mBlueBrush.get());
}

void VisualManager::OnDrawMenuCheck(
	CDC* dc, CMFCToolBarMenuButton* menuButton, CRect rect, BOOL highlighted, BOOL isRadio)
{
	CMFCToolBarImages& img = isRadio ? m_MenuItemMarkerR : m_MenuItemMarkerC;
	if (!CanDrawImage() || img.GetCount() == 0)
	{
		CMFCVisualManagerWindows::OnDrawMenuCheck(dc, menuButton, rect, highlighted, isRadio);
		return;
	}

	if (highlighted)
	{
		dc->FillRect(&rect, mBlueBrush.get());
	}
	else
	{
		dc->FillRect(&rect, mSemiWhiteBrush.get());
	}
	if ((menuButton->m_nStyle & TBBS_DISABLED) == TBBS_DISABLED)
	{
		mMenuChecks.Draw(dc, MENU_CHECK_D, rect.TopLeft(), 0);
	}
	else
	{
		mMenuChecks.Draw(dc, MENU_CHECK_E, rect.TopLeft(), 0);
	}
}

void VisualManager::OnDrawMenuBorder(CDC* dc, CMFCPopupMenu* popupMenu, CRect rect)
{
	dc->FillRect(&rect, mSemiWhiteBrush.get());

	BOOL isMenuBar = FALSE;
	CRect itemRect;
	CMFCToolBarMenuButton* menuButton = popupMenu->GetParentButton();
	if (menuButton != nullptr)
	{
		CWnd* parentWnd = menuButton->GetParentWnd();
		if (parentWnd != nullptr)
		{
			if (parentWnd->IsKindOf(RUNTIME_CLASS(CMFCMenuBar)))
			{
				isMenuBar = TRUE;
				itemRect = menuButton->Rect();
				parentWnd->ClientToScreen(&itemRect);
			}
		}
	}

	if (isMenuBar)
	{
		popupMenu->ScreenToClient(&itemRect);

		CPen* originalPen = dc->SelectObject(mLevel3Pen.get());

		dc->MoveTo(itemRect.right - 1, rect.top);
		dc->LineTo(rect.right, rect.top);
		dc->LineTo(rect.right, rect.bottom);
		dc->LineTo(rect.left, rect.bottom);
		dc->LineTo(rect.left, rect.top - 1);

		dc->SelectObject(originalPen);
	}
	else
	{
		auto borderColor = ToGdiColor(CLR_LEVEL3);
		dc->Draw3dRect(&rect, borderColor, borderColor);
	}
}

void VisualManager::OnDrawPaneBorder(CDC* dc, CBasePane* bar, CRect& rect)
{
	dc->FillRect(&rect, mBaseBrush.get());
}

void VisualManager::OnDrawPaneDivider(CDC* dc, CPaneDivider*, CRect rect, BOOL autoHideMode)
{
	dc->FillRect(&rect, mBaseBrush.get());
}

COLORREF VisualManager::OnFillMiniFrameCaption(CDC* dc, CRect rectCaption, CPaneFrameWnd*, BOOL isActive)
{
	mMultiFrameWndCaptionRect = rectCaption;

	dc->FillRect(&rectCaption, mBaseBrush.get());
	if (isActive)
	{
		return ToGdiColor(CLR_LEVEL1);
	}
	else
	{
		return ToGdiColor(CLR_LEVEL2);
	}
}

void VisualManager::OnDrawMiniFrameBorder(CDC* dc, CPaneFrameWnd*, CRect rectBorder, CRect rectBorderSize)
{
	dc->FillRect(&rectBorder, mBaseBrush.get());

	auto borderColor = ToGdiColor(CLR_LEVEL2);
	dc->Draw3dRect(&rectBorder, borderColor, borderColor);

	borderColor = ToGdiColor(CLR_LEVEL3);
	rectBorder.DeflateRect(1, 1);
	dc->Draw3dRect(&rectBorder, borderColor, borderColor);
}

COLORREF VisualManager::OnDrawPaneCaption(CDC* dc,
	CDockablePane* bar, BOOL isActive, CRect rectCaption, CRect rectButtons)
{
	mDockablePaneCaptionRect = rectCaption;

	if (isActive)
	{
		dc->FillRect(&rectCaption, mDarkBlueBrush.get());
		return ToGdiColor(CLR_WHITE);
	}
	else
	{
		dc->FillRect(&rectCaption, mLevel3Brush.get());
		return ToGdiColor(CLR_LEVEL1);
	}
}

void VisualManager::OnDrawCaptionButton(CDC* dc, CMFCCaptionButton* button,
	BOOL isActive, BOOL isHorz, BOOL maximized, BOOL disabled, int imageID)
{
	BOOL pushed = button->m_bPushed;
	BOOL focused = button->m_bFocused || button->m_bDroppedDown;

	CRect rect = button->GetRect();

	int tsState = TS_NORMAL;
	if (disabled)
	{
		tsState = TS_DISABLED;
	}
	else if (pushed && focused)
	{
		tsState = TS_PRESSED;
	}
	else if (focused)
	{
		tsState = TS_HOT;
	}

	POINT buttonPos;
	if (button->IsMiniFrameButton())
	{
		buttonPos.x = mMultiFrameWndCaptionRect.right - MINI_CLOSE_BUTTON_SIZE;
		buttonPos.y = mMultiFrameWndCaptionRect.top +
			(mMultiFrameWndCaptionRect.Height() - MINI_CLOSE_BUTTON_SIZE) / 2 - 1;
	}
	else
	{
		buttonPos.x = mDockablePaneCaptionRect.right - 18;
		buttonPos.y = (mDockablePaneCaptionRect.Height() - MINI_CLOSE_BUTTON_SIZE) / 2 + 1;
	}

	if (button->IsMiniFrameButton())
	{
		if (isActive)
		{
			if (tsState == TS_NORMAL || tsState == TS_DISABLED)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_DARK, buttonPos, 0);
			}
			else if (tsState == TS_HOT)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_GRAY, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_DARK, buttonPos, 0);
			}
		}
		else
		{
			if (tsState == TS_NORMAL || tsState == TS_DISABLED)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_LIGHT_GRAY, buttonPos, 0);
			}
			else if (tsState == TS_HOT)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_DARK, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_LIGHT_GRAY, buttonPos, 0);
			}
		}
	}
	else
	{
		if (isActive)
		{
			if (tsState == TS_NORMAL || tsState == TS_DISABLED)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_SEMI_WHITE, buttonPos, 0);
			}
			else if (tsState == TS_HOT)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_WHITE, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_SEMI_WHITE, buttonPos, 0);
			}
		}
		else
		{
			if (tsState == TS_NORMAL || tsState == TS_DISABLED)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_GRAY, buttonPos, 0);
			}
			else if (tsState == TS_HOT)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_WHITE, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_GRAY, buttonPos, 0);
			}
		}
	}
}
	
void VisualManager::GetTabFrameColors(const CMFCBaseTabCtrl* tabWnd,
	COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
	COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight,
	CBrush*& faceBrush, CBrush*& blackBrush)
{
	CMFCVisualManagerWindows7::GetTabFrameColors(tabWnd,
		clrDark, clrBlack, clrHighlight,
		clrFace, clrDarkShadow, clrLight,
		faceBrush, blackBrush);

	clrDark = ToGdiColor(CLR_BASE);
	clrBlack = ToGdiColor(CLR_BASE);
	clrDarkShadow = ToGdiColor(CLR_BASE);
	faceBrush = mBaseBrush.get();
	blackBrush = mBaseBrush.get();
}

int VisualManager::GetDockingTabsBordersSize()
{
	return 0;
}

void VisualManager::OnEraseTabsArea(CDC* dc, CRect rect, const CMFCBaseTabCtrl*)
{
	dc->FillRect(&rect, mBaseBrush.get());
}

void VisualManager::HandleMdiTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd)
{
	CRect ctrlRect;
	tabWnd->GetClientRect(&ctrlRect);
	dc->SelectClipRgn(nullptr);
	CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabWnd);
	assert(tabCtrl);

	CRect backgroundRect(rectTab);
	bool isMdiActive = mVisualDataProvider->IsMdiActive();
	BOOL isMdiActiveInTabGroup = tabCtrl->IsActiveInMDITabGroup();

	if (iTab == 0)
	{
		if (isActive || tabWnd->GetHighlightedTab() == iTab)
		{
			backgroundRect.left -= 2;
		}
	}
	backgroundRect.top -= 2;
	backgroundRect.right += 1;
	rectTab.OffsetRect(0, -2);

	COLORREF clrText = ToGdiColor(CLR_LEVEL1);

	// ----------------------------------------------------------------------------------------------------------------

	if (isActive)
	{
		if (isMdiActiveInTabGroup && isMdiActive)
		{
			clrText = ToGdiColor(CLR_WHITE);
			dc->FillRect(&backgroundRect, mDarkBlueBrush.get());
		}
		else
		{
			dc->FillRect(&backgroundRect, mDarkBaseBrush.get());
		}
	}
	else if (tabWnd->GetHighlightedTab() == iTab)
	{
		clrText = ToGdiColor(CLR_WHITE);
		dc->FillRect(&backgroundRect, mBlueBrush.get());
	}

	// ----------------------------------------------------------------------------------------------------------------

	if (isActive)
	{
		CRect rectClose = tabWnd->GetTabCloseButton();
		rectTab.right = rectClose.left - 1;

		BOOL isHighlighted = tabWnd->IsTabCloseButtonHighlighted();
		BOOL isPressed = tabWnd->IsTabCloseButtonPressed();

		int x = (rectClose.Width() - MINI_CLOSE_BUTTON_SIZE) / 2;
		int y = (rectClose.Height() - MINI_CLOSE_BUTTON_SIZE) / 2;
		if (x < 0 || y < 0)
		{
			return;
		}

		POINT buttonPos;
		buttonPos.x = rectClose.left + 1;
		buttonPos.y = rectClose.top - 1;
		if (isMdiActiveInTabGroup && isMdiActive)
		{
			if (isPressed)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_SEMI_WHITE, buttonPos, 0);
			}
			else if (isHighlighted)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_WHITE, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_SEMI_WHITE, buttonPos, 0);
			}
		}
		else
		{
			if (isPressed)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_GRAY, buttonPos, 0);
			}
			else if (isHighlighted)
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_WHITE, buttonPos, 0);
			}
			else
			{
				mMiniCloseButtons.Draw(dc, MINI_CLOSE_BUTTON_GRAY, buttonPos, 0);
			}
		}
	}

	CString strText;
	tabWnd->GetTabLabel(iTab, strText);

	CSize sizeImage = tabWnd->GetImageSize();
	UINT uiIcon = tabWnd->GetTabIcon(iTab);
	HICON hIcon = tabWnd->GetTabHicon(iTab);

	if (hIcon)
	{
		CRect rectImage = rectTab;

		rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
		rectImage.bottom = rectImage.top + sizeImage.cy;

		rectImage.left += AFX_IMAGE_MARGIN;
		rectImage.right = rectImage.left + sizeImage.cx;

		if (rectImage.top > 0)
		{
			dc->DrawState(rectImage.TopLeft(), rectImage.Size(), hIcon, DSS_NORMAL, static_cast<HBRUSH>(nullptr));
		}

		rectTab.left = rectImage.right;
	}
	else
	{
		CImageList* imageList = const_cast<CImageList*>(tabWnd->GetImageList());
		if (imageList != nullptr && uiIcon != (UINT)-1)
		{
			CRect rectImage = rectTab;

			rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + sizeImage.cy;

			rectImage.left += AFX_IMAGE_MARGIN;
			rectImage.right = rectImage.left + sizeImage.cx;

			if (rectImage.top > 0)
			{
				ASSERT_VALID(imageList);
				imageList->Draw(dc, uiIcon, rectImage.TopLeft(), ILD_TRANSPARENT);
			}

			rectTab.left = rectImage.right;
		}
	}

	//---------------
	// Draw tab text
	//---------------
	UINT textFormat = DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX;
	dc->SetTextColor(clrText);
	if (isActive)
	{
		CFont* originalFont = dc->SelectObject(mCommonFontR.get());
		dc->DrawText(strText, rectTab, textFormat);
		dc->SelectObject(originalFont);
	}
	else
	{
		dc->DrawText(strText, rectTab, textFormat);
	}

	// ----------------------------------------------------------------------------------------------------------------

	if (isActive)
	{
		CPen* originalPen;
		if (isMdiActiveInTabGroup && isMdiActive)
		{
			originalPen = dc->SelectObject(mDarkBluePen.get());
		}
		else
		{
			originalPen = dc->SelectObject(mDarkBasePen.get());
		}

		dc->MoveTo(0, rectTab.bottom);
		dc->LineTo(ctrlRect.right, rectTab.bottom);
		dc->MoveTo(0, rectTab.bottom + 1);
		dc->LineTo(ctrlRect.right, rectTab.bottom + 1);

		dc->SelectObject(originalPen);
	}

	// ----------------------------------------------------------------------------------------------------------------
}

void VisualManager::HandleDockingTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd)
{
	CRect ctrlRect;
	tabWnd->GetClientRect(&ctrlRect);
	dc->SelectClipRgn(nullptr);
	DockingTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(DockingTabCtrl, tabWnd);
	assert(tabCtrl);

	// ----------------------------------------------------------------------------------------------------------------

	if (isActive)
	{
		dc->FillRect(&rectTab, mDarkBaseBrush.get());
	}
	else if (tabWnd->GetHighlightedTab() == iTab)
	{
		dc->FillRect(&rectTab, mBlueBrush.get());
	}

	COLORREF clrText;
	if (isActive)
	{
		clrText = ToGdiColor(CLR_LEVEL1);
	}
	else if (tabWnd->GetHighlightedTab() == iTab)
	{
		clrText = ToGdiColor(CLR_WHITE);
	}
	else
	{
		clrText = ToGdiColor(CLR_LEVEL1);
	}
	
	// ----------------------------------------------------------------------------------------------------------------
		
	CString strText;
	tabWnd->GetTabLabel(iTab, strText);

	CSize sizeImage = tabWnd->GetImageSize();
	UINT uiIcon = tabWnd->GetTabIcon(iTab);
	HICON hIcon = tabWnd->GetTabHicon(iTab);

	if (uiIcon == (UINT)-1 && hIcon == nullptr)
	{
		sizeImage.cx = 0;
	}

	if (sizeImage.cx + 2 * CMFCBaseTabCtrl::AFX_TAB_IMAGE_MARGIN <= rectTab.Width())
	{
		if (hIcon != nullptr)
		{
			CRect rectImage = rectTab;

			rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
			rectImage.bottom = rectImage.top + sizeImage.cy;

			rectImage.left += AFX_IMAGE_MARGIN;
			rectImage.right = rectImage.left + sizeImage.cx;

			dc->DrawState(rectImage.TopLeft(), rectImage.Size(), hIcon, DSS_NORMAL, static_cast<HBRUSH>(nullptr));
		}
		else
		{
			CImageList* imageList = const_cast<CImageList*>(tabWnd->GetImageList());
			if (imageList != nullptr && uiIcon != (UINT)-1)
			{
				CRect rectImage = rectTab;

				rectImage.top += (rectTab.Height() - sizeImage.cy) / 2;
				rectImage.bottom = rectImage.top + sizeImage.cy;

				rectImage.left += AFX_IMAGE_MARGIN;
				rectImage.right = rectImage.left + sizeImage.cx;

				ASSERT_VALID(imageList);
				imageList->Draw(dc, uiIcon, rectImage.TopLeft(), ILD_TRANSPARENT);
			}
		}

		CRect rcText = rectTab;
		rcText.left += sizeImage.cx + 2 * AFX_TEXT_MARGIN;

		if (rcText.Width() < sizeImage.cx * 2 && !tabWnd->IsLeftRightRounded())
		{
			rcText.right -= AFX_TEXT_MARGIN;
		}

		dc->SetTextColor(clrText);
		UINT textFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
		rcText.OffsetRect(1, 0);
		dc->DrawText(strText, rcText, textFormat);
	}

	// ----------------------------------------------------------------------------------------------------------------
	
	CRect exRect;
	CBrush* exBrush;
	if (iTab == 0)
	{
		bool extend = false;
		if (isActive)
		{
			extend = true;
			exBrush = mDarkBaseBrush.get();
		}
		else if (tabWnd->GetHighlightedTab() == 0)
		{
			extend = true;
			exBrush = mBlueBrush.get();
		}
		if (extend)
		{
			exRect.left = 0;
			exRect.right = 2;
			exRect.top = rectTab.top;
			exRect.bottom = rectTab.bottom;
			dc->FillRect(&exRect, exBrush);

			exRect.left = rectTab.right;
			exRect.right = exRect.left + 1;
			dc->FillRect(&exRect, exBrush);
		}
	}
	else
	{
		bool extend = false;
		if (isActive)
		{
			extend = true;
			exBrush = mDarkBaseBrush.get();
		}
		else if (tabWnd->GetHighlightedTab() == iTab)
		{
			extend = true;
			exBrush = mBlueBrush.get();
		}
		if (extend)
		{
			exRect.left = rectTab.right;
			exRect.right = exRect.left + 1;
			exRect.top = rectTab.top;
			exRect.bottom = rectTab.bottom;
			dc->FillRect(&exRect, exBrush);
		}
	}

	if (isActive)
	{
		exRect.left = 0;
		exRect.right = ctrlRect.right;
		exRect.top = rectTab.top;
		exRect.bottom = exRect.top + 2;
		dc->FillRect(&exRect, mDarkBaseBrush.get());
	}

	// ----------------------------------------------------------------------------------------------------------------
}

void VisualManager::OnDrawTab(CDC* dc, CRect rectTab, int iTab, BOOL isActive, const CMFCBaseTabCtrl* tabWnd)
{
	if (tabWnd->IsMDITab())
	{
		HandleMdiTab(dc, rectTab, iTab, isActive, tabWnd);
	}
	else
	{
		HandleDockingTab(dc, rectTab, iTab, isActive, tabWnd);
	}
}

void VisualManager::OnDrawTabResizeBar(CDC* dc, CMFCBaseTabCtrl*, BOOL isVert, CRect rect, CBrush*, CPen*)
{
	if (isVert) 	rect.left -= 1;
	else			rect.top -= 1;
	dc->FillRect(&rect, mBaseBrush.get());
}

AFX_SMARTDOCK_THEME VisualManager::GetSmartDockingTheme()
{
	return AFX_SDT_VS2005;
}

} // namespace hsui
