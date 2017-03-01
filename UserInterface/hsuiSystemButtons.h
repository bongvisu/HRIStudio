#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API SystemButton : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	virtual ~SystemButton();

	static const int FIXED_BUTTON_WIDTH;
	static const int FIXED_BUTTON_HEIGHT;
	void CreateWnd(CWnd* parent);

	void MoveWnd(int x, int y);

	virtual BOOL PreTranslateMessage(MSG* msg) override;

protected:
	SystemButton();

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();

protected:
	CToolTipCtrl* mToolTip;

	enum ButtonState
	{
		BUTTON_STATE_NORMAL = 0,
		BUTTON_STATE_HOVERED,
		BUTTON_STATE_PRESSED,
		NUMBER_OF_BUTTON_STATES
	};
	ButtonState mButtonState;
	CPoint mMousePosition;

	xsc::D2DRenderer mRenderer;
};

class HSUI_API SystemButtonMinimize : public SystemButton
{
	DECLARE_MESSAGE_MAP()

public:
	SystemButtonMinimize();
	virtual ~SystemButtonMinimize();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* wnd);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	ID2D1Bitmap1* mImages[NUMBER_OF_BUTTON_STATES];
};

class HSUI_API SystemButtonMaxRestore : public SystemButton
{
	DECLARE_MESSAGE_MAP()

public:
	SystemButtonMaxRestore();
	virtual ~SystemButtonMaxRestore();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* wnd);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	bool mMaximized;
	ID2D1Bitmap1* mMaxImages[NUMBER_OF_BUTTON_STATES];
	ID2D1Bitmap1* mResImages[NUMBER_OF_BUTTON_STATES];
};

class HSUI_API SystemButtonClose : public SystemButton
{
	DECLARE_MESSAGE_MAP()

public:
	SystemButtonClose();
	virtual ~SystemButtonClose();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* wnd);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	ID2D1Bitmap1* mImages[NUMBER_OF_BUTTON_STATES];
};

} // namespace hsui
