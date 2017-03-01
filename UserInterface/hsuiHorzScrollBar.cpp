#include "hsuiPrecompiled.h"
#include "hsuiHorzScrollBar.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>

using namespace std;
using namespace xsc;

namespace hsui {

BEGIN_MESSAGE_MAP(HorzScrollBar, ScrollBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

ScrollBar::Type HorzScrollBar::GetType() const
{
	return TYPE_HORZ;
}

HorzScrollBar::HorzScrollBar()
{
	mThumbRect.top = CONTENT_MARGIN;
	mThumbRect.bottom = FIXED_SIZE - CONTENT_MARGIN;
}

HorzScrollBar::~HorzScrollBar()
{
}

void HorzScrollBar::CreateWnd(CWnd* parent, int x, int y, int wh)
{
	assert(GetSafeHwnd() == nullptr);
	CRect rect(x, y, x + wh, y + FIXED_SIZE);
	Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void HorzScrollBar::ResizeWnd(int wh)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, 0, 0, wh, FIXED_SIZE, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void HorzScrollBar::MoveAndResizeWnd(int x, int y, int wh)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, wh, FIXED_SIZE, SWP_NOZORDER | SWP_NOACTIVATE);
}

void HorzScrollBar::UpdateScrollParams()
{
	if (mEnd == mStart)
	{
		mWorldToView = 1;

		mThumbRect.left = FIXED_SIZE;
		mThumbRect.right = mThumbRect.left + mViewSize;

		return;
	}

	double worldRange = mEnd - mStart;
	int thumbSize = (int)(mViewSize * (mViewSize / (worldRange + mViewSize)));
	mWorldToView = (mViewSize - thumbSize) / worldRange;

	int scrollDistance = mScrollPosition - mStart;
	int viewSpaceScrollDistance = (int)(scrollDistance * mWorldToView);

	if (thumbSize < MIN_THUMB_SIZE) thumbSize = MIN_THUMB_SIZE;
	mThumbRect.left = FIXED_SIZE + viewSpaceScrollDistance;
	mThumbRect.right = mThumbRect.left + thumbSize;
	if (mThumbRect.right > (FIXED_SIZE + mViewSize))
	{
		int offset = FIXED_SIZE + mViewSize - mThumbRect.right;
		mThumbRect.left += offset;
		mThumbRect.right += offset;
	}
}

int HorzScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ScrollBar::OnCreate(lpCreateStruct);

	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_LN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowNN);
	
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_LH);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowNH);
	
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_RN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowFN);
	
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_RH);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowFH);

	mViewSize = lpCreateStruct->cx - 2 * FIXED_SIZE;
	UpdateScrollParams();

	return 0;
}

void HorzScrollBar::OnSize(UINT nType, int cx, int cy)
{
	ScrollBar::OnSize(nType, cx, cy);

	mViewSize = cx - 2 * FIXED_SIZE;
	UpdateScrollParams();
}

void HorzScrollBar::UpdateActivityState(CPoint point)
{
	CRect rect(0, 0, mViewSize + 2 * FIXED_SIZE, FIXED_SIZE);

	if (rect.PtInRect(point) == FALSE)
	{
		if (mActivityState != ACTIVITY_NONE)
		{
			mActivityState = ACTIVITY_NONE;
			Invalidate(FALSE);
		}
		return;
	}

	Activity oldState = mActivityState;

	rect.right = FIXED_SIZE;
	if (rect.PtInRect(point))
	{
		mActivityState = ACTIVITY_NEAR;
	}
	else
	{
		rect.left = FIXED_SIZE + mViewSize;
		rect.right = rect.left + FIXED_SIZE;
		if (rect.PtInRect(point))
		{
			mActivityState = ACTIVITY_FAR;
		}
		else if (mThumbRect.PtInRect(point))
		{
			mActivityState = ACTIVITY_THUMB;
		}
		else
		{
			mActivityState = ACTIVITY_PAGE;
			if (point.x < mThumbRect.left)	mPagingDirection = ACTIVITY_NEAR;
			else							mPagingDirection = ACTIVITY_FAR;
		}
	}
	
	if (oldState != mActivityState)
	{
		Invalidate(FALSE);
	}
}

void HorzScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	ScrollBar::OnMouseMove(nFlags, point);

	if (GetCapture() != this || mViewSize <= 0 || mActivityStatePivot != ACTIVITY_THUMB)
	{
		return;
	}

	int viewDelta = point.x - mThumbDownPosition.x;
	double worldDelta = viewDelta / mWorldToView;
	int oldScrollPosition = mScrollPosition;
	mScrollPosition = (int)(mPivotScrollPosition + worldDelta);
	ValidateScrollPosition();
	if (mScrollPosition != oldScrollPosition)
	{
		UpdateScrollParams();
		Render();

		if (mListener) mListener->OnScrollPositionChanged(this);
	}
}

void HorzScrollBar::Render()
{
	D2D1_RECT_F rect;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	rect.left = (FLOAT)mThumbRect.left;
	rect.top = (FLOAT)mThumbRect.top;
	rect.right = (FLOAT)mThumbRect.right;
	rect.bottom = (FLOAT)mThumbRect.bottom;
	if (mActivityState == ACTIVITY_THUMB || mActivityStatePivot == ACTIVITY_THUMB)
	{
		mRenderer.FillRectangle(rect, mBrush2);
	}
	else
	{
		mRenderer.FillRectangle(rect, mBrush1);
	}

	rect.top = 0.0f;
	rect.bottom = (FLOAT)FIXED_SIZE;

	rect.left = 0.0f;
	rect.right = (FLOAT)FIXED_SIZE;
	if (mActivityState == ACTIVITY_NEAR)
	{
		mRenderer.DrawBitmap(mArrowNH, &rect);
	}
	else
	{
		mRenderer.DrawBitmap(mArrowNN, &rect);
	}

	rect.left = (FLOAT)mViewSize + FIXED_SIZE;
	rect.right = rect.left + FIXED_SIZE;
	if (mActivityState == ACTIVITY_FAR)
	{
		mRenderer.DrawBitmap(mArrowFH, &rect);
	}
	else
	{
		mRenderer.DrawBitmap(mArrowFN, &rect);
	}
	mRenderer.EndDraw();

	mRenderer.Display();
}

} // namespace hsui
