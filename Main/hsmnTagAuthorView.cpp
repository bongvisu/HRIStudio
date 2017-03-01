#include "hsmnPrecompiled.h"
#include "hsmnTagAuthorView.h"
#include "hsmnApplication.h"
#include <xscDWrite.h>
#include <xscWicBitmap.h>
#include <hsmoModelManager.h>
#include <hsuiVisualManager.h>
#include <hsuiEditCtrl.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int TAV_FIXED_WIDTH = 600;
static const int TAV_FIXED_HEIGHT = 194;

static const int BORDER_SIZE = 3;
static const int CAPTION_SIZE = 30;
static const int CONTENT_MARGIN = 2;

BEGIN_MESSAGE_MAP(TagAuthorView, CWnd)
	ON_WM_NCACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

TagAuthorView::TagAuthorView()
{
	mActive = FALSE;
}

TagAuthorView::~TagAuthorView()
{
}

void TagAuthorView::OnClicked(ButtonCtrl* buttonCtrl)
{
	if (&mGenerate == buttonCtrl)
	{
		mEdit.SetContent(L"<mget color>");
	}
	else
	{
		mEdit.CopyToClipboard();
	}
}

void TagAuthorView::CreateWnd(CWnd* owner)
{
	const auto& modelName = theModelManager->GetActiveModel()->GetName();
	CRect rect(0, 0, TAV_FIXED_WIDTH, TAV_FIXED_HEIGHT);
	CreateEx(0, AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, LoadCursorW(nullptr, IDC_ARROW)),
		L"Tag Author", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, owner, 0);

	CenterWindow();
}

BOOL TagAuthorView::OnNcActivate(BOOL active)
{
	mActive = active;
	Invalidate(FALSE);
	return CWnd::OnNcActivate(active);
}

LRESULT TagAuthorView::OnNcHitTest(CPoint point)
{
	CRect wndRect;
	GetWindowRect(&wndRect);
	CRect rect;

	rect = wndRect;
	rect.right -= BORDER_SIZE;
	rect.bottom = rect.top + CAPTION_SIZE;
	if (rect.PtInRect(point)) return HTCAPTION;

	return CWnd::OnNcHitTest(point);
}

int TagAuthorView::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush({ 0, 0, 0, 1 }, &mBrush);

	WicBitmap wicBitmap;
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_TAG_AUTHOR_ICON);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIcon);

	mSysCloseButton.CreateWnd(this);

	static const int LABEL_WIDTH = 200;
	ComboCtrl::Item comboItem;

	int x = CONTENT_MARGIN + BORDER_SIZE;
	int y = CAPTION_SIZE + CONTENT_MARGIN + 1;
	mLabel1.CreateWnd(this, L"Data Category 1", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mLabel1.SetDrawBorder(true);

	x += LABEL_WIDTH + CONTENT_MARGIN;
	int w = TAV_FIXED_WIDTH - x - BORDER_SIZE - CONTENT_MARGIN;
	mCombo1.CreateWnd(this, x, y, w);
	comboItem.text = L"Test 1";
	mCombo1.AddItem(comboItem);
	comboItem.text = L"Test 2";
	mCombo1.AddItem(comboItem);
	comboItem.text = L"Test 3";
	mCombo1.AddItem(comboItem);
	mCombo1.SetCurrentItem(0);

	x = CONTENT_MARGIN + BORDER_SIZE;
	y += EditCtrl::FIXED_HEIGHT + CONTENT_MARGIN;
	mLabel2.CreateWnd(this, L"Data Category 2", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mLabel2.SetDrawBorder(true);

	x += LABEL_WIDTH + CONTENT_MARGIN;
	w = TAV_FIXED_WIDTH - x - BORDER_SIZE - CONTENT_MARGIN;
	mCombo2.CreateWnd(this, x, y, w);
	comboItem.text = L"Test 1";
	mCombo2.AddItem(comboItem);
	comboItem.text = L"Test 2";
	mCombo2.AddItem(comboItem);
	comboItem.text = L"Test 3";
	mCombo2.AddItem(comboItem);
	mCombo2.SetCurrentItem(0);

	x = CONTENT_MARGIN + BORDER_SIZE;
	y += EditCtrl::FIXED_HEIGHT + CONTENT_MARGIN;
	mLabel3.CreateWnd(this, L"Data Category 3", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mLabel3.SetDrawBorder(true);

	x += LABEL_WIDTH + CONTENT_MARGIN;
	w = TAV_FIXED_WIDTH - x - BORDER_SIZE - CONTENT_MARGIN;
	mCombo3.CreateWnd(this, x, y, w);
	comboItem.text = L"Test 1";
	mCombo3.AddItem(comboItem);
	comboItem.text = L"Test 2";
	mCombo3.AddItem(comboItem);
	comboItem.text = L"Test 3";
	mCombo3.AddItem(comboItem);
	mCombo3.SetCurrentItem(0);

	w = TAV_FIXED_WIDTH - 2 * BORDER_SIZE - 3 * CONTENT_MARGIN;
	w /= 2;
	w -= 1;

	x = CONTENT_MARGIN + BORDER_SIZE;
	y += EditCtrl::FIXED_HEIGHT + 2 * CONTENT_MARGIN;
	mGenerate.CreateWnd(this, L"Generate", x, y, w, EditCtrl::FIXED_HEIGHT);
	mGenerate.SetMode(ButtonCtrl::MODE_TEXT);

	x += w + CONTENT_MARGIN + 2;
	mCopy.CreateWnd(this, L"Copy", x, y, w, EditCtrl::FIXED_HEIGHT);
	mCopy.SetMode(ButtonCtrl::MODE_TEXT);

	x = CONTENT_MARGIN + BORDER_SIZE;
	y += EditCtrl::FIXED_HEIGHT + 2 * CONTENT_MARGIN;
	w = TAV_FIXED_WIDTH - 2 * BORDER_SIZE - 2 * CONTENT_MARGIN;
	int h = TAV_FIXED_HEIGHT - y - BORDER_SIZE - CONTENT_MARGIN;
	mEdit.CreateWnd(this, x, y, w, h, false, false, false);

	mGenerate.SetListener(static_cast<ButtonCtrl::IListener*>(this));
	mCopy.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	return 0;
}

void TagAuthorView::OnDestroy()
{
	mGenerate.ResetListener();
	mCopy.ResetListener();

	mIcon->Release();
	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void TagAuthorView::PostNcDestroy()
{
	delete this;
}

void TagAuthorView::OnClose()
{
	ShowWindow(SW_HIDE);
}

void TagAuthorView::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);

	if (mSysCloseButton.GetSafeHwnd())
	{
		int x = cx - SystemButton::FIXED_BUTTON_WIDTH - BORDER_SIZE;
		mSysCloseButton.MoveWnd(x, BORDER_SIZE);
		mSysCloseButton.Invalidate(FALSE);
	}
}

BOOL TagAuthorView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void TagAuthorView::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(clientRect.left);
	rect.top = static_cast<FLOAT>(clientRect.top);
	rect.right = static_cast<FLOAT>(clientRect.right);
	rect.bottom = static_cast<FLOAT>(clientRect.bottom);

	if (mActive)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL3);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_DARK_BASE);
	}
	rect.left += 1;
	rect.top += 1;
	mRenderer.DrawRectangle(rect, mBrush);

	CString wndText;
	GetWindowTextW(wndText);

	rect.left = BORDER_SIZE + 2.0f;;
	rect.top = 7;
	rect.right = rect.left + 20;
	rect.bottom = rect.top + 20;
	mRenderer.DrawBitmap(mIcon, rect);

	rect.left = rect.right + 2;
	rect.top = 1;
	rect.right = rect.left + 500;
	rect.bottom = rect.top + CAPTION_SIZE + 1;
	mBrush->SetColor(VisualManager::CLR_LEVEL1);
	mRenderer.DrawText(wndText, wndText.GetLength(), DWrite::segoe12B, rect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
