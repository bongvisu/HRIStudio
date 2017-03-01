#pragma once

#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API LabelCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	LabelCtrl();
	virtual ~LabelCtrl();

	void CreateWnd(CWnd* parent, const CString& text, int x, int y, int w, int h);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w, int h);
	void MoveAndResizeWnd(int x, int y, int w, int h);

	void SetBackgroundColor(const D2D1_COLOR_F& color);
	void SetForegroundColor(const D2D1_COLOR_F& color);

	void SetDrawBorder(bool draw);
	void SetBorderColor(const D2D1_COLOR_F& color);

	enum TextSize { TEST_SIZE_SMALL, TEXT_SIZE_MEDIUM, TEXT_SIZE_BIG };
	void SetTextSize(TextSize textSize);
	void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment);

	void SetBackgroundImage(const xsc::WicBitmap& wicBitmap);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	bool mDrawBorder;
	D2D1_COLOR_F mBorderColor;

	D2D1_COLOR_F mBackgroundColor;
	D2D1_COLOR_F mForegroundColor;
	TextSize mTextSize;
	DWRITE_TEXT_ALIGNMENT mTextAlignment;

	xsc::D2DRenderer mRenderer;

	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap1* mBackgroundImage;
};

} // namespace hsui
