#include "hsuiPrecompiled.h"
#include "hsuiRadioCtrl.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

static const int RADIO_ICON_SIZE = 16;

const int RadioCtrl::FIXED_HEIGHT = 22;

BEGIN_MESSAGE_MAP(RadioCtrl, CWnd)
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

RadioCtrl::RadioCtrl()
{
	mListener = nullptr;
	
	mSelected = false;

	mHot = false;
	mTrackingMouseLeave = false;

	mBackgroundImage = nullptr;
	mImagesE[0] = nullptr;
	mImagesE[1] = nullptr;
	mImagesD[0] = nullptr;
	mImagesD[1] = nullptr;
	mBrush = nullptr;
}

RadioCtrl::~RadioCtrl()
{
}

void RadioCtrl::SetListener(RadioCtrl::IListener* listener)
{
	mListener = listener;
}

void RadioCtrl::ResetListener()
{
	mListener = nullptr;
}

void RadioCtrl::CreateWnd(CWnd* parent, const CString& text, int x, int y, int w)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect(x, y, x + w, y + FIXED_HEIGHT);
	Create(nullptr, text, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void RadioCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void RadioCtrl::ResizeWnd(int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void RadioCtrl::MoveAndResizeWnd(int x, int y, int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
}

void RadioCtrl::SetSelected(bool selected, bool redraw)
{
	if (mSelected != selected)
	{
		mSelected = selected;
		if (GetSafeHwnd() && redraw) Invalidate(FALSE);
	}
}

bool RadioCtrl::GetSelected() const
{
	return mSelected;
}

int RadioCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrush);

	WicBitmap wicBitmap(Module::handle, HSUI_IDR_RADIO_BUTTON_ES);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesE[0]));
	
	wicBitmap.LoadFromResource(Module::handle, HSUI_IDR_RADIO_BUTTON_EU);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesE[1]));

	wicBitmap.LoadFromResource(Module::handle, HSUI_IDR_RADIO_BUTTON_DS);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesD[0]));

	wicBitmap.LoadFromResource(Module::handle, HSUI_IDR_RADIO_BUTTON_DU);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImagesD[1]));

	return 0;
}

void RadioCtrl::OnDestroy()
{
	SafeRelease(mBackgroundImage);
	SafeRelease(mBrush);
	SafeRelease(mImagesE[0]);
	SafeRelease(mImagesE[1]);
	SafeRelease(mImagesD[0]);
	SafeRelease(mImagesD[1]);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void RadioCtrl::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

void RadioCtrl::OnMouseMove(UINT nFlags, CPoint point)
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

void RadioCtrl::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	mHot = false;
	Invalidate(FALSE);
}

void RadioCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mSelected == false)
	{
		mSelected = true;
		Invalidate(FALSE);

		if (mListener) mListener->OnButtonSelected(this);
	}
}

void RadioCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (MK_LBUTTON & nFlags)
	{
		OnLButtonDown(nFlags, point);
	}
}

void RadioCtrl::OnEnable(BOOL bEnable)
{
	Invalidate(FALSE);
}

BOOL RadioCtrl::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void RadioCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CD2DRectF rect(clientRect);

	bool enabled = (GetStyle() & WS_DISABLED) ? false : true;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	if (enabled)
	{
		if (mHot)
		{
			mBrush->SetColor(VisualManager::VisualManager::CLR_DARK_BLUE);
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

	rect.top = (FIXED_HEIGHT - RADIO_ICON_SIZE) / 2;
	rect.bottom = rect.top + RADIO_ICON_SIZE;
	rect.left = 4;
	rect.right = rect.left + RADIO_ICON_SIZE;
	if (enabled)	mRenderer.DrawBitmap(mImagesE[mSelected ? 0 : 1], &rect);
	else			mRenderer.DrawBitmap(mImagesD[mSelected ? 0 : 1], &rect);

	CString wndText;
	GetWindowText(wndText);

	rect.left = rect.right + 3;
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
