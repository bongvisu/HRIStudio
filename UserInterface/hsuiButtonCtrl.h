#pragma once

#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API ButtonCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnClicked(ButtonCtrl* buttonCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

public:
	ButtonCtrl();
	virtual ~ButtonCtrl();

	enum Mode { MODE_IMAGE, MODE_TEXT };
	void SetMode(Mode mode);

	void CreateWnd(CWnd* parent, const CString& text, int x, int y, int w, int h);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w, int h);
	void MoveAndResizeWnd(int x, int y, int w, int h);

	void SetBackgroundImage(const xsc::WicBitmap& image);
	void SetBackgroundColor(const D2D1_COLOR_F& color);

	void SetNormalImage(const xsc::WicBitmap& image);
	void SetHotImage(const xsc::WicBitmap& image);
	void SetPressedImage(const xsc::WicBitmap& image);
	void SetDisabledImage(const xsc::WicBitmap& image);

	void SetRounded(bool rounded);
	void SetNormalColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center);
	void SetHotColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center);
	void SetPressedColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center);
	void SetTextColor(const D2D1_COLOR_F& normal, const D2D1_COLOR_F& hot, const D2D1_COLOR_F& pressed);

	void ActivateToolTip();
	void DeactivateToolTip();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnEnable(BOOL enable);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	
	void RenderImageMode(CRect clientRect);
	void RenderTextMode(CRect clientRect);
	
	enum State { STATE_NORMAL = 0, STATE_HOT, STATE_PRESSED, NUMBER_OF_STATES };
	void SetupGradientBrush(State state);

protected:
	IListener* mListener;
	CToolTipCtrl* mToolTip;

	Mode mMode;
	State  mButtonState;
	CPoint mMousePosition;
	
	xsc::D2DRenderer mRenderer;
	
	D2D1_COLOR_F mBackgroundColor;
	ID2D1Bitmap1* mBackgroundImage;
	ID2D1Bitmap1* mImages[NUMBER_OF_STATES];
	ID2D1Bitmap1* mDisabledImage;

	bool mRounded;
	ID2D1SolidColorBrush* mBrush;
	D2D1_COLOR_F mOuterColors[NUMBER_OF_STATES];
	D2D1_COLOR_F mCenterColors[NUMBER_OF_STATES];
	D2D1_COLOR_F mTextColors[NUMBER_OF_STATES];
	ID2D1LinearGradientBrush* mGradientBrushes[NUMBER_OF_STATES];
};

} // namespace hsui
