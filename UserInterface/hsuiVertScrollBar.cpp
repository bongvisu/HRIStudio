#include "hsuiPrecompiled.h"
#include "hsuiVertScrollBar.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>

using namespace std;
using namespace xsc;

namespace hsui {

BEGIN_MESSAGE_MAP(VertScrollBar, ScrollBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

ScrollBar::Type VertScrollBar::GetType() const
{
	return TYPE_VERT;
}

VertScrollBar::VertScrollBar()
{
	mThumbRect.left = CONTENT_MARGIN;
	mThumbRect.right = FIXED_SIZE - CONTENT_MARGIN;
}

VertScrollBar::~VertScrollBar()
{
}

void VertScrollBar::CreateWnd(CWnd* parent, int x, int y, int wh)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect(x, y, x + FIXED_SIZE, y + wh);
	Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void VertScrollBar::ResizeWnd(int wh)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, 0, 0, FIXED_SIZE, wh, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void VertScrollBar::MoveAndResizeWnd(int x, int y, int wh)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, FIXED_SIZE, wh, SWP_NOZORDER | SWP_NOACTIVATE);
}

void VertScrollBar::UpdateScrollParams()
{
	if (mEnd == mStart)
	{
		mWorldToView = 1;

		mThumbRect.top = FIXED_SIZE;
		mThumbRect.bottom = mThumbRect.top + mViewSize;

		return;
	}

	double worldRange = mEnd - mStart;
	int thumbSize = (int)(mViewSize * (mViewSize / (worldRange + mViewSize)));
	mWorldToView = (mViewSize - thumbSize) / worldRange;

	int scrollDistance = mScrollPosition - mStart;
	int viewSpaceScrollDistance = (int)(scrollDistance * mWorldToView);

	if (thumbSize < MIN_THUMB_SIZE) thumbSize = MIN_THUMB_SIZE;
	mThumbRect.top = FIXED_SIZE + viewSpaceScrollDistance;
	mThumbRect.bottom = mThumbRect.top + (int)thumbSize;
	if (mThumbRect.bottom > (FIXED_SIZE + mViewSize))
	{
		int offset = FIXED_SIZE + mViewSize - mThumbRect.bottom;
		mThumbRect.top += offset;
		mThumbRect.bottom += offset;
	}
}

int VertScrollBar::OnCreate(LPCREATESTRUCT createStruct)
{
	ScrollBar::OnCreate(createStruct);

	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_UN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowNN);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_UH);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowNH);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_DN);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowFN);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SCROLL_ARROW_DH);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowFH);

	mViewSize = createStruct->cy - 2 * FIXED_SIZE;
	UpdateScrollParams();

	return 0;
}

void VertScrollBar::OnSize(UINT type, int cx, int cy)
{
	ScrollBar::OnSize(type, cx, cy);

	mViewSize = cy - 2 * FIXED_SIZE;
	UpdateScrollParams();
}

void VertScrollBar::UpdateActivityState(CPoint point)
{
	CRect rect(0, 0, FIXED_SIZE, mViewSize + 2 * FIXED_SIZE);

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

	rect.bottom = FIXED_SIZE;
	if (rect.PtInRect(point))
	{
		mActivityState = ACTIVITY_NEAR;
	}
	else
	{
		rect.top = FIXED_SIZE + mViewSize;
		rect.bottom = rect.top + FIXED_SIZE;
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
			if (point.y < mThumbRect.top)	mPagingDirection = ACTIVITY_NEAR;
			else							mPagingDirection = ACTIVITY_FAR;
		}
	}
	
	if (oldState != mActivityState)
	{
		Invalidate(FALSE);
	}
}

void VertScrollBar::OnMouseMove(UINT flags, CPoint point)
{
	ScrollBar::OnMouseMove(flags, point);

	if (GetCapture() != this || mViewSize <= 0 || mActivityStatePivot != ACTIVITY_THUMB)
	{
		return;
	}

	int viewDelta = point.y - mThumbDownPosition.y;
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

void VertScrollBar::Render()
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

	rect.left = 0.0f;
	rect.right = (FLOAT)FIXED_SIZE;

	rect.top = 0.0f;
	rect.bottom = (FLOAT)FIXED_SIZE;
	if (mActivityState == ACTIVITY_NEAR)
	{
		mRenderer.DrawBitmap(mArrowNH, &rect);
	}
	else
	{
		mRenderer.DrawBitmap(mArrowNN, &rect);
	}

	rect.top = (FLOAT)mViewSize + FIXED_SIZE;
	rect.bottom = rect.top + FIXED_SIZE;
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
