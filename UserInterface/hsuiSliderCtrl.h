#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API SliderCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnValueChanged(SliderCtrl* sliderCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

public:
	SliderCtrl(bool horz = true);
	virtual ~SliderCtrl();

	static const int FIXED_SIZE;
	void CreateWnd(CWnd* parent, int x, int y, int wh);

	void MoveWnd(int x, int y);
	void ResizeWnd(int wh);
	void MoveAndResizeWnd(int x, int y, int wh);

	void SetRange(double start, double end);
	double RangeStart() const;
	double RangeEnd() const;
	
	void SetValue(double value);
	double GetValue() const;

	void SetDisplayPercentage(bool usePercentage);
	bool GetDisplayPercentage() const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnEnable(BOOL enable);
	afx_msg BOOL OnSetCursor(CWnd* wnd, UINT hitTest, UINT message);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	void Render();

	bool UpdateLogicalValue(double value);
	void UpdateThumbRect();
	double ViewToLogicalSpan(int viewSize, int signedSpan) const;
	
protected:
	IListener* mListener;

	bool mHorizontal;
	bool mDisplayPercentage;

	double mValue;
	double mStart;
	double mEnd;
	
	int mClientWidth;
	int mClientHeight;

	CRect mThumbRect;
	bool mHot;
	bool mThumbHit;
	CPoint mPivotPos;
	double mPivotValue;
	bool mTrackingMouseLeave;
	HCURSOR mResizingCursor;

	xsc::D2DRenderer mRenderer;
	ID2D1Bitmap1* mThumbImageE;
	ID2D1Bitmap1* mThumbImageD;
	ID2D1SolidColorBrush* mBrushTextNormal;
	ID2D1SolidColorBrush* mBrushTextBright;
	ID2D1SolidColorBrush* mBrushFillE;
	ID2D1SolidColorBrush* mBrushFillD;
	ID2D1SolidColorBrush* mBrushBorderHot;
	ID2D1SolidColorBrush* mBrushBorderNormal;
};

} // namespace mco
