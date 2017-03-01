#include "hsuiPrecompiled.h"
#include "hsuiDialog.h"
#include "hsuiResource.h"
#include "hsuiVisualManager.h"

#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

const int Dialog::FIXED_BORDER_SIZE = 3;
const int Dialog::FIXED_CAPTION_HEIGHT = 30;

BEGIN_MESSAGE_MAP(Dialog, CDialogEx)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

Dialog::Dialog() : CDialogEx(HSUI_IDR_COMMON_DIALOG_TEMPLATE)
{
	mW = 640;
	mH = 480;

	mPresentationUpdater = dynamic_cast<IApplication*>(AfxGetApp());
}

Dialog::~Dialog()
{
}

void Dialog::OnWindowPosChanged(WINDOWPOS* windowPos)
{
	CDialogEx::OnWindowPosChanged(windowPos);

	if (mShadowBorders[0].GetSafeHwnd())
	{
		HDWP hdwp = BeginDeferWindowPos(1);

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
}

LRESULT Dialog::OnNcHitTest(CPoint point)
{
	CRect rect;
	GetWindowRect(&rect);
	
	rect.left += FIXED_BORDER_SIZE;
	rect.right -= FIXED_BORDER_SIZE;
	rect.top += FIXED_BORDER_SIZE;
	rect.bottom = rect.top + FIXED_CAPTION_HEIGHT;
	if (rect.PtInRect(point))
	{
		return HTCAPTION;
	}

	return CDialogEx::OnNcHitTest(point);
}

int Dialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDialogEx::OnCreate(lpCreateStruct);

	for (int i = 0; i < ShadowBorder::NUMBER_OF_ROLES; ++i)
	{
		mShadowBorders[i].CreateWnd(this, static_cast<ShadowBorder::Role>(i), false);
	}

	mSysCloseButton.CreateWnd(this);

	SetBackgroundColor(VisualManager::ToGdiColor(VisualManager::CLR_BASE));

	mBorderPenOuter.CreatePen(PS_SOLID, 1, VisualManager::ToGdiColor(VisualManager::CLR_LEVEL2));
	mBorderPenInner.CreatePen(PS_SOLID, 1, VisualManager::ToGdiColor(VisualManager::CLR_LEVEL3));

	WicBitmap wicBitmap(theVisualManager->GetAppInstanceHandle(), theVisualManager->GetAppIconActive());
	D2DRenderer::dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIcon);
	D2DRenderer::dcRenderTarget->CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mBrush);

	return 0;
}

BOOL Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowPos(nullptr, 0, 0, mW, mH, SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOACTIVATE);
	CenterWindow();

	for (int i = 0; i < ShadowBorder::NUMBER_OF_ROLES; ++i)
	{
		mShadowBorders[i].ShowWindow(SW_SHOW);
	}

	InitWindow();

	return TRUE;
}

void Dialog::OnDestroy()
{
	mBrush->Release();
	mIcon->Release();

	mBorderPenOuter.DeleteObject();
	mBorderPenInner.DeleteObject();

	CDialogEx::OnDestroy();
}

void Dialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (mSysCloseButton.GetSafeHwnd())
	{
		int x = cx - SystemButton::FIXED_BUTTON_WIDTH - FIXED_BORDER_SIZE;
		mSysCloseButton.MoveWnd(x, FIXED_BORDER_SIZE);
		mSysCloseButton.Invalidate(FALSE);
	}
}

void Dialog::OnPaint()
{
	CPaintDC dc(this);

	CRect clientRect;
	GetClientRect(&clientRect);

	CPen* oldPen;

	oldPen = dc.SelectObject(&mBorderPenOuter);

	dc.MoveTo(0, 0);
	dc.LineTo(clientRect.right - 1, 0);
	dc.LineTo(clientRect.right - 1, clientRect.bottom - 1);
	dc.LineTo(0, clientRect.bottom - 1);
	dc.LineTo(0, 0);

	dc.SelectObject(&mBorderPenInner);
	dc.MoveTo(2, 2);
	dc.LineTo(clientRect.right - 3, 2);
	dc.LineTo(clientRect.right - 3, clientRect.bottom - 3);
	dc.LineTo(2, clientRect.bottom - 3);
	dc.LineTo(2, 2);

	dc.MoveTo(2, FIXED_CAPTION_HEIGHT - 1);
	dc.LineTo(clientRect.right - 3, FIXED_CAPTION_HEIGHT - 1);

	dc.SelectObject(oldPen);

	PaintContent(&dc);

	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;
	dcRenderTarget->BindDC(dc, &clientRect);
	dcRenderTarget->BeginDraw();

	D2D1_RECT_F d2dRect;

	d2dRect.left = FIXED_BORDER_SIZE + 4.0f;
	d2dRect.top = FIXED_BORDER_SIZE + 1.0f;
	d2dRect.right = d2dRect.left + 24;
	d2dRect.bottom = d2dRect.top + 24;
	dcRenderTarget->DrawBitmap(mIcon, d2dRect);

	CString wndText;
	GetWindowTextW(wndText);

	d2dRect.left = d2dRect.right + 4;
	d2dRect.right = (FLOAT)clientRect.right;
	dcRenderTarget->DrawText(wndText, wndText.GetLength(), DWrite::segoe12B, d2dRect, mBrush);

	dcRenderTarget->EndDraw();
}

void Dialog::PaintContent(CDC* dc)
{
}

} // namespace hsui
