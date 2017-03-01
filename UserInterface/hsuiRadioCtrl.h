#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API RadioCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnButtonSelected(RadioCtrl* radioCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

public:
	RadioCtrl();
	virtual ~RadioCtrl();

	static const int FIXED_HEIGHT;
	void CreateWnd(CWnd* parent, const CString& text, int x, int y, int w);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w);
	void MoveAndResizeWnd(int x, int y, int w);

	void SetSelected(bool selected, bool redraw = true);
	bool GetSelected() const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg void OnEnable(BOOL enable);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	IListener* mListener;

	bool mSelected;
	bool mHot;
	bool mTrackingMouseLeave;

	xsc::D2DRenderer mRenderer;
	ID2D1Bitmap1* mBackgroundImage;
	ID2D1Bitmap1* mImagesE[2];
	ID2D1Bitmap1* mImagesD[2];
	ID2D1SolidColorBrush* mBrush;
};

} // namespace hsui
