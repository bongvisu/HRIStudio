#include "hsuiPrecompiled.h"
#include "hsuiCheckCtrl.h"
#include "hsuiResource.h"
#include "hsuiVisualManager.h"
#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

static const int CHECK_ICON_SIZE = 16;

const int CheckCtrl::FIXED_HEIGHT = 22;

BEGIN_MESSAGE_MAP(CheckCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CheckCtrl::CheckCtrl()
{
	mListener = nullptr;

	mSelected = false;
	mHot = false;
	mTrackingMouseLeave = false;

	mBrush = nullptr;
	mImagesE[0] = mImagesE[1] = nullptr;
	mImagesD[0] = mImagesD[1] = nullptr;
}

CheckCtrl::~CheckCtrl()
{
}

void CheckCtrl::SetListener(CheckCtrl::IListener* listener)
{
	mListener = listener;
}

void CheckCtrl::ResetListener()
{
	mListener = nullptr;
}

void CheckCtrl::CreateWnd(CWnd* parent, const CString& text, int x, int y, int w)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect(x, y, x + w, y + FIXED_HEIGHT);
	Create(nullptr, text, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void CheckCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CheckCtrl::ResizeWnd(int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void CheckCtrl::MoveAndResizeWnd(int x, int y, int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
}

void CheckCtrl::SetSelected(bool selected, bool redraw)
{
	if (selected != mSelected)
	{
		mSelected = selected;

		if (GetSafeHwnd() && redraw)
		{
			Invalidate(FALSE);
		}
	}
}

bool CheckCtrl::GetSelected() const
{
	return mSelected;
}

int CheckCtrl::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrush);
	
	WicBitmap wicBitmap(hsui::Module::handle, HSUI_IDR_CHECK_ICON_EP);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesE[0]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_CHECK_ICON_EN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesE[1]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_CHECK_ICON_DP);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesD[0]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_CHECK_ICON_DN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesD[1]));

	return 0;
}

void CheckCtrl::OnDestroy()
{
	SafeRelease(mBrush);
	SafeRelease(mImagesE[0]);
	SafeRelease(mImagesE[1]);
	SafeRelease(mImagesD[0]);
	SafeRelease(mImagesD[1]);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void CheckCtrl::OnSize(UINT type, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

void CheckCtrl::OnMouseMove(UINT flags, CPoint point)
{
	if (mTrackingMouseLeave == false)
	{
		mTrackingMouseLeave = true;

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		TrackMouseEvent(&trackMouseEvent);
	}

	if (mHot == false)
	{
		mHot = true;
		Invalidate(FALSE);
	}
}

void CheckCtrl::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	mHot = false;
	Invalidate(FALSE);
}

void CheckCtrl::OnLButtonDown(UINT flags, CPoint point)
{
	if (mSelected)	mSelected = false;
	else			mSelected = true;
	Invalidate(FALSE);

	if (mListener) mListener->OnCheckStateChanged(this);
}

void CheckCtrl::OnLButtonDblClk(UINT flags, CPoint point)
{
	if (MK_LBUTTON & flags)
	{
		OnLButtonDown(flags, point);
	}
}

void CheckCtrl::OnEnable(BOOL enable)
{
	Invalidate(FALSE);
}

BOOL CheckCtrl::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void CheckCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CD2DRectF rect(clientRect);

	bool enabled = (GetStyle() & WS_DISABLED) ? false : true;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);

	if (enabled)
	{
		if (mHot)
		{
			mBrush->SetColor(VisualManager::CLR_DARK_BLUE);
		}
		else
		{
			mBrush->SetColor(VisualManager::CLR_LEVEL3);
		}
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_BASE);
	}
	rect.left = 1;
	rect.top = 1;
	mRenderer.DrawRectangle(rect, mBrush);

	rect.top = (FIXED_HEIGHT - CHECK_ICON_SIZE) / 2;
	rect.bottom = rect.top + CHECK_ICON_SIZE;
	rect.left = 3;
	rect.right = rect.left + CHECK_ICON_SIZE;
	if (enabled)	mRenderer.DrawBitmap(mImagesE[mSelected ? 0 : 1], &rect);
	else			mRenderer.DrawBitmap(mImagesD[mSelected ? 0 : 1], &rect);

	CString wndText;
	GetWindowText(wndText);

	rect.left = rect.right + 2;
	rect.right = (FLOAT)clientRect.right;
	rect.top = (FLOAT)(clientRect.top - 1);
	rect.bottom = (FLOAT)(clientRect.bottom - 1);
	if (enabled)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL1);
		mRenderer.DrawText(wndText, wndText.GetLength(), DWrite::segoe12R, rect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL2);
		mRenderer.DrawText(wndText, wndText.GetLength(), DWrite::segoe12R, rect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}

	mRenderer.EndDraw();

	mRenderer.Display();
}

} // namespace hsui
