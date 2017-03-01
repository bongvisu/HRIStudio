#include "hsmnPrecompiled.h"
#include "hsmnCodeView.h"
#include "hsmnApplication.h"
#include <xscDWrite.h>
#include <xscWicBitmap.h>
#include <hsmoModelManager.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int BORDER_SIZE = 3;
static const int CAPTION_SIZE = 30;

BEGIN_MESSAGE_MAP(CodeView, CWnd)
	ON_WM_NCACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CodeView::CodeView()
{
	mActive = FALSE;
}

CodeView::~CodeView()
{
}

void CodeView::Update(Model* model)
{
	auto modelName(theModelManager->GetActiveModel()->GetName() + L" - CODE");
	SetWindowTextW(modelName.c_str());

	wstring code;
	model->Compile(code);
	mEdit.SetContent(code);
}

void CodeView::UpdateAndDownload(Model* model)
{
	auto modelName(theModelManager->GetActiveModel()->GetName() + L" - CODE");
	SetWindowTextW(modelName.c_str());

	wstring code;
	if (model->Compile(code))
	{
		theModelManager->DownloadCode(code);
	}
	mEdit.SetContent(code);
}

void CodeView::CreateWnd(CWnd* owner)
{
	static const int CV_DEFAULT_WIDTH = 800;
	static const int CV_DEFAULT_HEIGHT = 600;

	CRect rect(0, 0, CV_DEFAULT_WIDTH, CV_DEFAULT_HEIGHT);
	CreateEx(0,
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,  LoadCursorW(nullptr, IDC_ARROW)),
		L"CODE", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, owner, 0);

	CenterWindow();
}

BOOL CodeView::OnNcActivate(BOOL active)
{
	mActive = active;
	Invalidate(FALSE);
	return CWnd::OnNcActivate(active);
}

LRESULT CodeView::OnNcHitTest(CPoint point)
{
	CRect wndRect;
	GetWindowRect(&wndRect);
	CRect rect;

	rect = wndRect;
	rect.right = rect.left + BORDER_SIZE;
	rect.bottom = rect.top + BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOPLEFT;

	rect = wndRect;
	rect.left = rect.right - BORDER_SIZE;
	rect.top = rect.bottom - BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOMRIGHT;

	rect = wndRect;
	rect.right = rect.left + BORDER_SIZE;
	rect.top = rect.bottom - BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOMLEFT;

	rect = wndRect;
	rect.left = rect.right - BORDER_SIZE;
	rect.bottom = rect.top + BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOPRIGHT;

	rect = wndRect;
	rect.bottom = rect.top + BORDER_SIZE;
	if (rect.PtInRect(point)) return HTTOP;

	rect = wndRect;
	rect.right = rect.left + BORDER_SIZE;
	if (rect.PtInRect(point)) return HTLEFT;

	rect = wndRect;
	rect.left = rect.right - BORDER_SIZE;
	if (rect.PtInRect(point)) return HTRIGHT;

	rect = wndRect;
	rect.top = rect.bottom - BORDER_SIZE;
	if (rect.PtInRect(point)) return HTBOTTOM;

	rect = wndRect;
	rect.right -= BORDER_SIZE;
	rect.bottom = rect.top + CAPTION_SIZE;
	if (rect.PtInRect(point)) return HTCAPTION;

	return CWnd::OnNcHitTest(point);
}

int CodeView::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush({ 0, 0, 0, 1 }, &mBrush);

	WicBitmap wicBitmap;
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_CAPTION_ICON_ACTIVE);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIcon);

	mSysCloseButton.CreateWnd(this);

	mEdit.CreateWnd(this, 0, 0, 100, 100);

	return 0;
}

void CodeView::OnDestroy()
{
	mIcon->Release();
	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void CodeView::PostNcDestroy()
{
	delete this;
}

void CodeView::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CodeView::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);

	if (mSysCloseButton.GetSafeHwnd())
	{
		int x = cx - SystemButton::FIXED_BUTTON_WIDTH - BORDER_SIZE;
		mSysCloseButton.MoveWnd(x, BORDER_SIZE);
		mSysCloseButton.Invalidate(FALSE);
	}

	if (mEdit.GetSafeHwnd())
	{
		mEdit.MoveWindow(BORDER_SIZE, CAPTION_SIZE + 2,
			cx - 2 * BORDER_SIZE, cy - CAPTION_SIZE - BORDER_SIZE - 2, FALSE);
	}
}

BOOL CodeView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CodeView::OnPaint()
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
	rect.top = 6;
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
