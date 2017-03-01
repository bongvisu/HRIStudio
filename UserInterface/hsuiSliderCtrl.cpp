#include "hsuiPrecompiled.h"
#include "hsuiSliderCtrl.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

static const int FIXED_THUMB_SIZE = 20;
static const int HALF_THUMB_SIZE = 10;

const int SliderCtrl::FIXED_SIZE = 22;

BEGIN_MESSAGE_MAP(SliderCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

SliderCtrl::SliderCtrl(bool horz)
{
	mListener = nullptr;

	mHorizontal = horz;

	mDisplayPercentage = false;

	if (mHorizontal)
	{
		mThumbRect.top = 1;
		mThumbRect.bottom = mThumbRect.top + FIXED_SIZE - 2;
	}
	else
	{
		mThumbRect.left = 1;
		mThumbRect.right = mThumbRect.left + FIXED_SIZE - 2;
	}

	mValue = 50;
	mStart = 0;
	mEnd = 100;

	mHot = false;
	mThumbHit = false;
	mTrackingMouseLeave = false;

	mThumbImageE = nullptr;
	mThumbImageD = nullptr;
	mBrushTextNormal = nullptr;
	mBrushTextBright = nullptr;
	mBrushFillE = nullptr;
	mBrushFillD = nullptr;
	mBrushBorderHot = nullptr;
	mBrushBorderNormal = nullptr;
}

SliderCtrl::~SliderCtrl()
{
}

void SliderCtrl::SetListener(SliderCtrl::IListener* listener)
{
	mListener = listener;
}

void SliderCtrl::ResetListener()
{
	mListener = nullptr;
}

void SliderCtrl::CreateWnd(CWnd* parent, int x, int y, int wh)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect;
	if (mHorizontal)
	{
		rect.left = x;
		rect.right = x + wh;
		rect.top = y;
		rect.bottom = y + FIXED_SIZE;
	}
	else
	{
		rect.left = x;
		rect.right = x + FIXED_SIZE;
		rect.top = y;
		rect.bottom = y + wh;
	}
	Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void SliderCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void SliderCtrl::ResizeWnd(int wh)
{
	assert(GetSafeHwnd());

	if (mHorizontal)
	{
		SetWindowPos(nullptr, 0, 0, wh, FIXED_SIZE, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos(nullptr, 0, 0, FIXED_SIZE, wh, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

void SliderCtrl::MoveAndResizeWnd(int x, int y, int wh)
{
	assert(GetSafeHwnd());

	if (mHorizontal)
	{
		SetWindowPos(nullptr, x, y, wh, FIXED_SIZE, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos(nullptr, x, y, FIXED_SIZE, wh, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void SliderCtrl::SetRange(double start, double end)
{
	assert(GetSafeHwnd());

	if (end <= start)
	{
		if (mStart != 0 || mEnd != 0 || mValue != 0)
		{
			mStart = mEnd = mValue = 0;
			UpdateThumbRect();
			Invalidate(FALSE);
		}
		return;
	}

	if (start == mStart && end == mEnd)
	{
		return;
	}

	mStart = start;
	mEnd = end;

	if (mValue < mStart)
	{
		mValue = mStart;
	}
	else if (mValue > mEnd)
	{
		mValue = mEnd;
	}

	UpdateThumbRect();

	Render();
}

double SliderCtrl::RangeStart() const
{
	return mStart;
}

double SliderCtrl::RangeEnd() const
{
	return mEnd;
}

void SliderCtrl::UpdateThumbRect()
{
	if (mStart == mEnd)
	{
		if (mHorizontal)
		{
			mThumbRect.left = 1;
			mThumbRect.right = mThumbRect.left + FIXED_THUMB_SIZE;
		}
		else
		{
			mThumbRect.top = 1;
			mThumbRect.bottom = mThumbRect.top + FIXED_THUMB_SIZE;
		}
		return;
	}

	int viewSize = mHorizontal ? mClientWidth : mClientHeight;
	double viewCenter = viewSize * 0.5;
	viewSize -= FIXED_THUMB_SIZE + 2;

	double logicalRange = mEnd - mStart;
	double logicalToView = viewSize / logicalRange;
	double logicalCenter = (mStart + mEnd) * 0.5;
	double centeredDelta = logicalToView * (mValue - logicalCenter);
	if (mHorizontal)
	{
		mThumbRect.left = (int)(viewCenter + centeredDelta - HALF_THUMB_SIZE);
		if (mThumbRect.left < 1) mThumbRect.left = 1;
		mThumbRect.right = mThumbRect.left + FIXED_THUMB_SIZE;
	}
	else
	{
		mThumbRect.top = (int)(viewCenter + centeredDelta - HALF_THUMB_SIZE);
		if (mThumbRect.top < 1) mThumbRect.top = 1;
		mThumbRect.bottom = mThumbRect.top + FIXED_THUMB_SIZE;
	}
}

bool SliderCtrl::UpdateLogicalValue(double value)
{
	double oldValue = mValue;

	if		(value < mStart)	mValue = mStart;
	else if (value > mEnd)		mValue = mEnd;
	else						mValue = value;

	if (oldValue != mValue)
	{
		UpdateThumbRect();
		Render();
		return true;
	}
	return false;
}

double SliderCtrl::ViewToLogicalSpan(int viewSize, int signedSpan) const
{
	if (viewSize <= 0)
	{
		return 0;
	}

	double logicalRange = mEnd - mStart;
	double viewToLogical = logicalRange / viewSize;
	return (viewToLogical * signedSpan);
}

void SliderCtrl::SetValue(double value)
{
	assert(GetSafeHwnd());

	UpdateLogicalValue(value);
}

double SliderCtrl::GetValue() const
{
	return mValue;
}

void SliderCtrl::SetDisplayPercentage(bool usePercentage)
{
	if (mDisplayPercentage != usePercentage)
	{
		mDisplayPercentage = usePercentage;
		if (GetSafeHwnd())
		{
			Invalidate(FALSE);
		}
	}
}

bool SliderCtrl::GetDisplayPercentage() const
{
	return mDisplayPercentage;
}

int SliderCtrl::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mClientWidth = createStruct->cx;
	mClientHeight = createStruct->cy;

	if (mHorizontal)
	{
		mResizingCursor = LoadCursor(Module::handle, MAKEINTRESOURCE(HSUI_IDR_RESIZE_CURSOR_V));
	}
	else
	{
		mResizingCursor = LoadCursor(Module::handle, MAKEINTRESOURCE(HSUI_IDR_RESIZE_CURSOR_H));
	}

	mRenderer.Create(m_hWnd, mClientWidth, mClientHeight);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	if (mHorizontal)
	{
		WicBitmap wicBitmap(Module::handle, HSUI_IDR_SLIDER_THUMB_HE);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbImageE);
		wicBitmap.LoadFromResource(Module::handle, HSUI_IDR_SLIDER_THUMB_HD);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbImageD);
	}
	else
	{
		WicBitmap wicBitmap(Module::handle, HSUI_IDR_SLIDER_THUMB_VE);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbImageE);

		wicBitmap.LoadFromResource(Module::handle, HSUI_IDR_SLIDER_THUMB_VD);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbImageD);
	}

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mBrushTextNormal);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_WHITE, &mBrushTextBright);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL2, &mBrushFillE);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrushFillD);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_DARK_BLUE, &mBrushBorderHot);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrushBorderNormal);

	return 0;
}

void SliderCtrl::OnDestroy()
{
	SafeRelease(mThumbImageE);
	SafeRelease(mThumbImageD);
	SafeRelease(mBrushTextNormal);
	SafeRelease(mBrushTextBright);
	SafeRelease(mBrushFillE);
	SafeRelease(mBrushFillD);
	SafeRelease(mBrushBorderHot);
	SafeRelease(mBrushBorderNormal);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void SliderCtrl::OnSize(UINT type, int cx, int cy)
{
	mClientWidth = cx;
	mClientHeight = cy;

	UpdateThumbRect();

	mRenderer.Resize(cx, cy);
}

void SliderCtrl::OnEnable(BOOL enable)
{
	Invalidate(FALSE);
}

BOOL SliderCtrl::OnSetCursor(CWnd* wnd, UINT hitTest, UINT message)
{
	if (mThumbHit)
	{
		SetCursor(mResizingCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(wnd, hitTest, message);
}

void SliderCtrl::OnMouseMove(UINT flags, CPoint point)
{
	if (GetCapture() == this)
	{
		int viewSize, viewDelta;
		if (mHorizontal)
		{
			viewSize = mClientWidth - FIXED_THUMB_SIZE - 2;
			viewDelta = point.x - mPivotPos.x;
		}
		else
		{
			viewSize = mClientHeight - FIXED_THUMB_SIZE - 2;
			viewDelta = point.y - mPivotPos.y;
		}
		double logicalDelta = ViewToLogicalSpan(viewSize, viewDelta);
		if (UpdateLogicalValue(mPivotValue + logicalDelta))
		{
			if (mListener) mListener->OnValueChanged(this);
		}
	}
	else
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

		if (mThumbRect.PtInRect(point))
		{
			if (mThumbHit == false)
			{
				mThumbHit = true;
				SetCursor(mResizingCursor);
			}
		}
		else
		{
			mThumbHit = false;
		}
	}

	if (mHot == false)
	{
		mHot = true;
		Invalidate(FALSE);
	}
}

void SliderCtrl::OnMouseLeave()
{
	mTrackingMouseLeave = false;
	mThumbHit = false;
	mHot = false;
	Invalidate(FALSE);
}

void SliderCtrl::OnLButtonDown(UINT flags, CPoint point)
{
	if (mThumbRect.PtInRect(point))
	{
		mThumbHit = true;
	}
	else
	{
		mThumbHit = false;
	}

	if (mThumbHit)
	{
		mPivotPos = point;
		mPivotValue = mValue;

		SetCursor(mResizingCursor);

		SetCapture();
	}
	else
	{
		int viewSize, viewSpan;
		if (mHorizontal)
		{
			viewSize = mClientWidth - FIXED_THUMB_SIZE - 2;
			viewSpan = point.x - (mThumbRect.left + HALF_THUMB_SIZE);
		}
		else
		{
			viewSize = mClientHeight - FIXED_THUMB_SIZE - 2;
			viewSpan = point.y - (mThumbRect.top + HALF_THUMB_SIZE);
		}

		double logicalSpan = ViewToLogicalSpan(viewSize, viewSpan);

		if (UpdateLogicalValue(mValue + logicalSpan))
		{
			if (mListener) mListener->OnValueChanged(this);
		}
	}
}

void SliderCtrl::OnLButtonUp(UINT flags, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}

	CRect rect(0, 0, mClientWidth, mClientHeight);
	if (rect.PtInRect(point))
	{
		mHot = true;
		if (mThumbRect.PtInRect(point))
		{
			mThumbHit = true;
			SetCursor(mResizingCursor);
		}
		else
		{
			mThumbHit = false;
		}
	}
	else
	{
		mHot = false;
		mThumbHit = false;
	}

	Invalidate(FALSE);
}

void SliderCtrl::OnLButtonDblClk(UINT flags, CPoint point)
{
	if (MK_LBUTTON & flags)
	{
		OnLButtonDown(flags, point);
	}
}

BOOL SliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void SliderCtrl::OnPaint()
{
	CPaintDC dc(this);

	Render();
}

void SliderCtrl::Render()
{
	CD2DRectF rect(0, 0, static_cast<FLOAT>(mClientWidth), static_cast<FLOAT>(mClientHeight));

	bool enabled = (GetStyle() & WS_DISABLED) ? false : true;

	mRenderer.BeginDraw();
	if (enabled)	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);
	else			mRenderer.Clear(VisualManager::CLR_BASE);

	CD2DRectF thumbRect(mThumbRect);
	if (enabled)	mRenderer.DrawBitmap(mThumbImageE, &thumbRect);
	else			mRenderer.DrawBitmap(mThumbImageD, &thumbRect);

	CD2DRectF valueRect(mThumbRect);
	valueRect.left = 0;
	valueRect.right = (FLOAT)mThumbRect.left;
	if (enabled)	mRenderer.FillRectangle(valueRect, mBrushFillE);
	else			mRenderer.FillRectangle(valueRect, mBrushFillD);

	CString percentageStr;
	if (mStart == mEnd)
	{
		valueRect.left = (FLOAT)mThumbRect.right;
		valueRect.right = rect.right;
		percentageStr = L"N/A";
	}
	else
	{
		if (mDisplayPercentage)
		{
			double range = mEnd - mStart;
			double percentage = ((mValue - mStart) / range) * 100.0;
			percentageStr.Format(L"%.01f%%", percentage);
		}
		else
		{
			percentageStr.Format(L"%g", mValue);
		}
	}

	IDWriteTextFormat* font = DWrite::segoe14R;
	font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	valueRect.top -= 1;
	valueRect.bottom -= 1;
	valueRect.left = -1;
	valueRect.right = static_cast<FLOAT>(mClientWidth);
	if (enabled)
	{
		mRenderer.DrawText(percentageStr, percentageStr.GetLength(),
			font, valueRect, mBrushTextBright, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
	else
	{
		mRenderer.DrawText(percentageStr, percentageStr.GetLength(),
			font, valueRect, mBrushTextNormal, D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}

	font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	rect.top = 1;
	rect.left = 1;
	if (enabled)
	{
		if (mHot)	mRenderer.DrawRectangle(rect, mBrushBorderHot);
		else		mRenderer.DrawRectangle(rect, mBrushBorderNormal);
	}
	else
	{
		mRenderer.DrawRectangle(rect, mBrushBorderNormal);
	}

	mRenderer.EndDraw();

	mRenderer.Display();
}

} // namespace hsui
