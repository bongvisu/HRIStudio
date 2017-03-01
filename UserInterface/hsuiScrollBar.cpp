#include "hsuiPrecompiled.h"
#include "hsuiScrollBar.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"

using namespace std;
using namespace xsc;

namespace hsui {

const UINT ScrollBar::TMR_ID_SCROLL_BOOTSTRAP = 1;
const UINT ScrollBar::TMR_ID_SCROLL_PERSISTENT = 2;
const int ScrollBar::CONTENT_MARGIN = 3;
const int ScrollBar::MIN_THUMB_SIZE = 10;

const int ScrollBar::FIXED_SIZE = 17;

BEGIN_MESSAGE_MAP(ScrollBar, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ScrollBar::ScrollBar()
{
	mListener = nullptr;

	mStart = 0;
	mEnd = 0;
	mStepSize = 10;
	mPageSize = 100;

	mScrollPosition = 0;
	
	mActivityState = ACTIVITY_NONE;
	mTrackingMouseLeave = false;

	mBrush1  = nullptr;
	mBrush2  = nullptr;
	mArrowNN = nullptr;
	mArrowNH = nullptr;
	mArrowFN = nullptr;
	mArrowFH = nullptr;
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetListener(ScrollBar::IListener* listener)
{
	mListener = listener;
}

void ScrollBar::ResetListener()
{
	mListener = nullptr;
}

void ScrollBar::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ScrollBar::ValidateScrollPosition()
{
	if (mScrollPosition < mStart)
	{
		mScrollPosition = mStart;
	}
	else if (mScrollPosition > mEnd)
	{
		mScrollPosition = mEnd;
	}
}

void ScrollBar::SetRange(int start, int end)
{
	if (end < start) end = start;

	mStart = start;
	mEnd = end;

	ValidateScrollPosition();

	if (GetSafeHwnd())
	{
		UpdateScrollParams();
		Invalidate(FALSE);
	}
}

int ScrollBar::GetRangeStart() const
{
	return mStart;
}

int ScrollBar::GetRangeEnd() const
{
	return mEnd;
}

void ScrollBar::SetStepSize(int stepSize)
{
	if (stepSize < 0) stepSize = 0;
	mStepSize = stepSize;
}

int ScrollBar::GetStepSize() const
{
	return mStepSize;
}

void ScrollBar::SetPageSize(int pageSize)
{
	if (pageSize < 0) pageSize = 0;
	mPageSize = pageSize;
}

int ScrollBar::GetPageSize() const
{
	return mPageSize;
}

void ScrollBar::SetScrollPosition(int position)
{
	int oldScrollPosition = mScrollPosition;
	mScrollPosition = position;
	ValidateScrollPosition();

	if (mScrollPosition != oldScrollPosition && GetSafeHwnd())
	{
		UpdateScrollParams();

		Invalidate(FALSE);
	}
}

int ScrollBar::GetScrollPosition() const
{
	return mScrollPosition;
}

int ScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);

	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrush1);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL2, &mBrush2);

	return 0;
}

void ScrollBar::OnDestroy()
{
	SafeRelease(mBrush1);
	SafeRelease(mBrush2);
	SafeRelease(mArrowNN);
	SafeRelease(mArrowNH);
	SafeRelease(mArrowFN);
	SafeRelease(mArrowFH);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ScrollBar::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

void ScrollBar::OnMouseMove(UINT nFlags, CPoint point)
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

	UpdateActivityState(point);
}

void ScrollBar::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	KillTimer(TMR_ID_SCROLL_BOOTSTRAP);
	KillTimer(TMR_ID_SCROLL_PERSISTENT);

	mActivityState = ACTIVITY_NONE;
	Invalidate(FALSE);
}

bool ScrollBar::MoveScrollPosition(int delta)
{
	int oldScrollPosition = mScrollPosition;

	mScrollPosition += delta;
	ValidateScrollPosition();

	if (oldScrollPosition != mScrollPosition)
	{
		UpdateScrollParams();
		Invalidate(FALSE);
		return true;
	}
	return false;
}

void ScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	
	SetCapture();

	mActivityStatePivot = mActivityState;
	mPagingDirectionPivot = mPagingDirection;

	if (mActivityState == ACTIVITY_NEAR)
	{
		KillTimer(TMR_ID_SCROLL_PERSISTENT);
		SetTimer(TMR_ID_SCROLL_BOOTSTRAP, 500, nullptr);

		if (MoveScrollPosition(-mStepSize))
		{
			if (mListener) mListener->OnScrollPositionChanged(this);
		}
	}
	else if (mActivityState == ACTIVITY_FAR)
	{
		KillTimer(TMR_ID_SCROLL_PERSISTENT);
		SetTimer(TMR_ID_SCROLL_BOOTSTRAP, 500, nullptr);

		if (MoveScrollPosition(mStepSize))
		{
			if (mListener) mListener->OnScrollPositionChanged(this);
		}
	}
	else if (mActivityState == ACTIVITY_THUMB)
	{
		mThumbDownPosition = point;
		mPivotScrollPosition = mScrollPosition;
	}
	else if (mActivityState == ACTIVITY_PAGE)
	{
		KillTimer(TMR_ID_SCROLL_PERSISTENT);
		SetTimer(TMR_ID_SCROLL_BOOTSTRAP, 500, nullptr);

		DoPageScroll(point);
	}
}

void ScrollBar::DoPageScroll(CPoint point)
{
	if (mPagingDirection == ACTIVITY_NEAR)
	{	
		if (MoveScrollPosition(-mPageSize))
		{
			if (mListener) mListener->OnScrollPositionChanged(this);
		}
	}
	else
	{
		if (MoveScrollPosition(mPageSize))
		{
			if (mListener) mListener->OnScrollPositionChanged(this);
		}
	}
}

void ScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	mActivityStatePivot = ACTIVITY_NONE;
	Invalidate(FALSE);

	mTrackingMouseLeave = true;
	TRACKMOUSEEVENT trackMouseEvent;
	trackMouseEvent.cbSize = sizeof(trackMouseEvent);
	trackMouseEvent.dwFlags = TME_LEAVE;
	trackMouseEvent.hwndTrack = m_hWnd;
	TrackMouseEvent(&trackMouseEvent);

	KillTimer(TMR_ID_SCROLL_BOOTSTRAP);
	KillTimer(TMR_ID_SCROLL_PERSISTENT);
}

void ScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (MK_LBUTTON & nFlags)
	{
		OnLButtonDown(nFlags, point);
	}
}

void ScrollBar::OnTimer(UINT_PTR id)
{
	if (id == TMR_ID_SCROLL_BOOTSTRAP)
	{
		KillTimer(TMR_ID_SCROLL_BOOTSTRAP);
		
		if (mActivityState == ACTIVITY_NEAR ||
			mActivityState == ACTIVITY_FAR ||
			mActivityState == ACTIVITY_PAGE)
		{
			SetTimer(TMR_ID_SCROLL_PERSISTENT, 100, nullptr);
		}		
	}
	else if (id == TMR_ID_SCROLL_PERSISTENT)
	{
		if (mActivityState == ACTIVITY_NEAR && mActivityStatePivot == ACTIVITY_NEAR)
		{
			if (MoveScrollPosition(-mStepSize))
			{
				if (mListener) mListener->OnScrollPositionChanged(this);
			}
		}
		else if (mActivityState == ACTIVITY_FAR && mActivityStatePivot == ACTIVITY_FAR)
		{
			if (MoveScrollPosition(mStepSize))
			{
				if (mListener) mListener->OnScrollPositionChanged(this);
			}
		}
		else if (mActivityStatePivot == ACTIVITY_PAGE)
		{
			CPoint point;
			::GetCursorPos(&point);
			ScreenToClient(&point);
			UpdateActivityState(point);
			if (mActivityState == ACTIVITY_PAGE && mPagingDirectionPivot == mPagingDirection)
			{
				DoPageScroll(point);
			}
		}
	}
	else
	{
		CWnd::OnTimer(id);
	}
}

BOOL ScrollBar::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void ScrollBar::OnPaint()
{
	CPaintDC dc(this);
	
	Render();
}

} // namespace hsui
