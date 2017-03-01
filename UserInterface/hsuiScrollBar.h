#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API ScrollBar : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnScrollPositionChanged(ScrollBar* scrollBar) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

protected:
	ScrollBar();

public:
	enum Type { TYPE_HORZ = 0, TYPE_VERT, NUMBER_OF_TYPES };
	virtual Type GetType() const = 0;

	virtual ~ScrollBar() override;

	static const int FIXED_SIZE;
	virtual void CreateWnd(CWnd* parent, int x, int y, int wh) = 0;

	void MoveWnd(int x, int y);
	virtual void ResizeWnd(int wh) = 0;
	virtual void MoveAndResizeWnd(int x, int y, int wh) = 0;

	void SetRange(int start, int end);
	int GetRangeStart() const;
	int GetRangeEnd() const;
	
	void SetStepSize(int stepSize);
	int GetStepSize() const;
	void SetPageSize(int pageSize);
	int GetPageSize() const;

	void SetScrollPosition(int position);
	int GetScrollPosition() const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg void OnTimer(UINT_PTR id);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

	void ValidateScrollPosition();
	bool MoveScrollPosition(int delta);
	void DoPageScroll(CPoint point);

	virtual void UpdateScrollParams() = 0;
	virtual void UpdateActivityState(CPoint point) = 0;
	virtual void Render() = 0;

protected:
	IListener* mListener;

	int mStart;
	int mEnd;
	int mStepSize;
	int mPageSize;
	int mScrollPosition;
	
	CRect mThumbRect;
	int mViewSize;
	double mWorldToView;

	enum Activity
	{
		ACTIVITY_NONE,
		ACTIVITY_PAGE,
		ACTIVITY_NEAR,
		ACTIVITY_FAR,
		ACTIVITY_THUMB
	};
	Activity mActivityState;
	Activity mPagingDirection;
	Activity mActivityStatePivot;
	Activity mPagingDirectionPivot;

	bool mTrackingMouseLeave;
	CPoint mThumbDownPosition;
	int mPivotScrollPosition;

	xsc::D2DRenderer mRenderer;

	ID2D1SolidColorBrush* mBrush1;
	ID2D1SolidColorBrush* mBrush2;
	ID2D1Bitmap1* mArrowNN;
	ID2D1Bitmap1* mArrowNH;
	ID2D1Bitmap1* mArrowFN;
	ID2D1Bitmap1* mArrowFH;

	static const UINT TMR_ID_SCROLL_BOOTSTRAP;
	static const UINT TMR_ID_SCROLL_PERSISTENT;
	static const int CONTENT_MARGIN;
	static const int MIN_THUMB_SIZE;
};

} // namespace hsui
