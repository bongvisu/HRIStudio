#include "hsmnPrecompiled.h"
#include "hsmnToolStrip.h"
#include "hsmnApplication.h"
#include "hsmnDHSelectGroup.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int BUTTON_SIZE = 24;
static const int INTER_BUTTON_GAP = 8;
static const int FIXED_BUTTON_TOP = 4;

BEGIN_MESSAGE_MAP(ToolStrip, CMFCToolBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

ToolStrip::ToolStrip()
{
}

ToolStrip::~ToolStrip()
{
}

int ToolStrip::GetRowHeight() const
{
	return 38;
}

int ToolStrip::OnCreate(LPCREATESTRUCT createStruct)
{
	CMFCToolBar::OnCreate(createStruct);

	D2DRenderer::dcRenderTarget->CreateSolidColorBrush(VisualManager::CLR_LEVEL2, &mBrush);
		
	int x = 5;
	WicBitmap wicBitmap;

	mNew.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mNew.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_NEW_N);
	mNew.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_NEW_H);
	mNew.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_NEW_P);
	mNew.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mOpen.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mOpen.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_OPEN_N);
	mOpen.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_OPEN_H);
	mOpen.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_OPEN_P);
	mOpen.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x += 1;
	mSeparators.push_back(x);
	x += 7;

	mClose.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mClose.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CLOSE_N);
	mClose.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CLOSE_H);
	mClose.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CLOSE_P);
	mClose.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x += 1;
	mSeparators.push_back(x);
	x += 8;

	mSave.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mSave.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAVE_N);
	mSave.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAVE_H);
	mSave.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAVE_P);
	mSave.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x += 1;
	mSeparators.push_back(x);
	x += 5;

	mCut.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mCut.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CUT_N);
	mCut.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CUT_H);
	mCut.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_CUT_P);
	mCut.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mCopy.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mCopy.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COPY_N);
	mCopy.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COPY_H);
	mCopy.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COPY_P);
	mCopy.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mPaste.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mPaste.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_PASTE_N);
	mPaste.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_PASTE_H);
	mPaste.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_PASTE_P);
	mPaste.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x += 2;
	mSeparators.push_back(x);
	x += 5;

	mUndo.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mUndo.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_UNDO_N);
	mUndo.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_UNDO_H);
	mUndo.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_UNDO_P);
	mUndo.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mRedo.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mRedo.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_REDO_N);
	mRedo.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_REDO_H);
	mRedo.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_REDO_P);
	mRedo.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x -= 1;
	mSeparators.push_back(x);
	x += 6;

	mSelectionMode.CreateWnd(this, x, 5, 198);
	mSelectionMode.SetListener(static_cast<ComboCtrl::IListener*>(this));

	ComboCtrl::Item modeItem;

	modeItem.text = L"Group Selection : Blocks";
	mSelectionMode.AddItem(modeItem);
	
	modeItem.text = L"Group Selection : Joints";
	mSelectionMode.AddItem(modeItem);
	
	modeItem.text = L"Group Selection : Segments";
	mSelectionMode.AddItem(modeItem);
	
	modeItem.text = L"Group Selection : All";
	mSelectionMode.AddItem(modeItem);
	
	mSelectionMode.SetCurrentItem(static_cast<int>(DHSelectGroup::groupSelectionMode));

	x += 205;
	mSeparators.push_back(x);
	x += 6;

	mAlignLeft.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mAlignLeft.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_LEFT_N);
	mAlignLeft.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_LEFT_H);
	mAlignLeft.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_LEFT_P);
	mAlignLeft.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP + 1;

	mAlignRight.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mAlignRight.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_RIGHT_N);
	mAlignRight.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_RIGHT_H);
	mAlignRight.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_RIGHT_P);
	mAlignRight.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP + 1;

	mAlignTop.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mAlignTop.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_TOP_N);
	mAlignTop.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_TOP_H);
	mAlignTop.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_TOP_P);
	mAlignTop.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP + 1;

	mAlignBottom.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mAlignBottom.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_BOTTOM_N);
	mAlignBottom.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_BOTTOM_H);
	mAlignBottom.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_ALIGN_BOTTOM_P);
	mAlignBottom.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x += 2;
	mSeparators.push_back(x);
	x += 8;

	mSameSize.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mSameSize.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_SIZE_N);
	mSameSize.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_SIZE_H);
	mSameSize.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_SIZE_P);
	mSameSize.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mSameWidth.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mSameWidth.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_WIDTH_N);
	mSameWidth.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_WIDTH_H);
	mSameWidth.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_WIDTH_P);
	mSameWidth.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mSameHeight.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mSameHeight.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_HEIGHT_N);
	mSameHeight.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_HEIGHT_H);
	mSameHeight.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SAME_HEIGHT_P);
	mSameHeight.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	x -= 2;
	mSeparators.push_back(x);
	x += 8;

	mCompile.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mCompile.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COMPILE_N);
	mCompile.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COMPILE_H);
	mCompile.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_COMPILE_P);
	mCompile.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mDownload.CreateWnd(this, L"", x, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mDownload.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_RUN_N);
	mDownload.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_RUN_H);
	mDownload.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_RUN_P);
	mDownload.SetPressedImage(wicBitmap);
	x += BUTTON_SIZE + INTER_BUTTON_GAP;

	mSettings.CreateWnd(this, L"", 0, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mSettings.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SETTING_N);
	mSettings.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SETTING_H);
	mSettings.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_SETTING_P);
	mSettings.SetPressedImage(wicBitmap);

	mHelp.CreateWnd(this, L"", 0, FIXED_BUTTON_TOP, BUTTON_SIZE, BUTTON_SIZE);
	mHelp.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_HELP_N);
	mHelp.SetNormalImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_HELP_H);
	mHelp.SetHotImage(wicBitmap);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TOOL_BUTTON_HELP_P);
	mHelp.SetPressedImage(wicBitmap);

	return 0;
}

void ToolStrip::OnDestroy()
{
	mBrush->Release();

	CMFCToolBar::OnDestroy();
}

void ToolStrip::OnSize(UINT type, int cx, int cy)
{
	CMFCToolBar::OnSize(type, cx, cy);

	mSettings.MoveWnd(cx - 2 * BUTTON_SIZE - INTER_BUTTON_GAP - 4, FIXED_BUTTON_TOP);
	mHelp.MoveWnd(cx - BUTTON_SIZE - 4, FIXED_BUTTON_TOP);
}

void ToolStrip::OnSelectionChanged(ComboCtrl* comboCtrl)
{
	DHSelectGroup::groupSelectionMode = static_cast<GroupSelectionMode>(comboCtrl->GetCurrentItem());
}

void ToolStrip::OnClicked(ButtonCtrl* buttonCtrl)
{
	// File -----------------------------------------------------------------------------------------------------------
	if (&mNew == buttonCtrl)
	{
		theApp.OnFileNew();
	}
	else if (&mOpen == buttonCtrl)
	{
		theApp.OnFileOpen();
	}
	else if (&mClose == buttonCtrl)
	{
		theApp.OnFileClose();
	}
	else if (&mSave == buttonCtrl)
	{
		theApp.OnFileSave();
	}
	// Edit -----------------------------------------------------------------------------------------------------------
	else if (&mCut == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(ID_EDIT_CUT, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mCopy == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(ID_EDIT_COPY, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mPaste == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(ID_EDIT_PASTE, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mUndo == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(ID_EDIT_UNDO, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mRedo == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(ID_EDIT_REDO, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	// Format ---------------------------------------------------------------------------------------------------------
	else if (&mAlignLeft == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_ALIGN_LEFT, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mAlignRight == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_ALIGN_RIGHT, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mAlignTop == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_ALIGN_TOP, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mAlignBottom == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_ALIGN_BOTTOM, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mSameSize == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_MAKE_SAME_SIZE, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mSameWidth == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_MAKE_SAME_WIDTH, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mSameHeight == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_FORMAT_MAKE_SAME_HEIGHT, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	// Project --------------------------------------------------------------------------------------------------------
	else if (&mCompile == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_PROJECT_COMPILE, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	else if (&mDownload == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_PROJECT_DOWNLOAD, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	// Tool -----------------------------------------------------------------------------------------------------------
	else if (&mSettings == buttonCtrl)
	{
		WPARAM wParam = MAKEWPARAM(HSMN_IDC_TOOL_SETTINGS, 0);
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, wParam);
	}
	// Help -----------------------------------------------------------------------------------------------------------
	else if (&mHelp == buttonCtrl)
	{
		theApp.OnHelp();
	}
}

void ToolStrip::DoPaint(CDC* dcPaint)
{
	CRect rectClient;
	GetClientRect(rectClient);

	CMemDC memDC(*dcPaint, this);
	CDC* dc = &memDC.GetDC();

	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;
	dcRenderTarget->BindDC(dc->m_hDC, &rectClient);
	dcRenderTarget->BeginDraw();
	dcRenderTarget->Clear(VisualManager::CLR_BASE);

	D2D1_POINT_2F pt0, pt1;
	for (auto sep : mSeparators)
	{
		pt0.x = (FLOAT)sep;
		pt0.y = 4;
		pt1.x = pt0.x;
		pt1.y = (FLOAT)rectClient.bottom - 10;
		dcRenderTarget->DrawLine(pt0, pt1, mBrush);
	}

	dcRenderTarget->EndDraw();
}

} // namespace hsmn
