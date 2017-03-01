#include "hsuiPrecompiled.h"
#include "hsuiLabelCtrl.h"
#include "hsuiVisualManager.h"
#include <xscDWrite.h>

using namespace xsc;

namespace hsui {

BEGIN_MESSAGE_MAP(LabelCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LabelCtrl::LabelCtrl()
	:
	mBrush(nullptr),
	mBackgroundImage(nullptr),
	mTextSize(TEST_SIZE_SMALL),
	mTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER)
{
	mBackgroundColor = VisualManager::CLR_BASE;
	mForegroundColor = VisualManager::CLR_LEVEL1;

	mDrawBorder = false;
	mBorderColor = VisualManager::CLR_LEVEL3;
}

LabelCtrl::~LabelCtrl()
{
}

void LabelCtrl::CreateWnd(CWnd* parent, const CString& text, int x, int y, int w, int h)
{
	assert(GetSafeHwnd() == nullptr);
	
	CRect rect(x, y, x + w, y + h);
	Create(nullptr, text, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void LabelCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void LabelCtrl::ResizeWnd(int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void LabelCtrl::MoveAndResizeWnd(int x, int y, int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void LabelCtrl::SetBackgroundColor(const D2D1_COLOR_F& color)
{
	mBackgroundColor = color;

	if (GetSafeHwnd()) Invalidate(FALSE);
}

void LabelCtrl::SetForegroundColor(const D2D1_COLOR_F& color)
{
	mForegroundColor = color;

	if (GetSafeHwnd()) Invalidate(FALSE);
}

void LabelCtrl::SetDrawBorder(bool draw)
{
	if (mDrawBorder != draw)
	{
		mDrawBorder = draw;
		if (GetSafeHwnd()) Invalidate(FALSE);
	}
}

void LabelCtrl::SetBorderColor(const D2D1_COLOR_F& color)
{
	mBorderColor = color;
	if (GetSafeHwnd()) Invalidate(FALSE);
}

void LabelCtrl::SetBackgroundImage(const WicBitmap& wicBitmap)
{
	assert(GetSafeHwnd());

	SafeRelease(mBackgroundImage);

	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mBackgroundImage);

	Invalidate(FALSE);
}

void LabelCtrl::SetTextSize(TextSize textSize)
{
	if (mTextSize != textSize)
	{
		mTextSize = textSize;
		if (GetSafeHwnd()) Invalidate(FALSE);
	}
}

void LabelCtrl::SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment)
{
	if (mTextAlignment != textAlignment)
	{
		mTextAlignment = textAlignment;
		if (GetSafeHwnd()) Invalidate(FALSE);
	}
}

int LabelCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(mForegroundColor, &mBrush);

	return 0;
}

void LabelCtrl::OnDestroy()
{
	SafeRelease(mBackgroundImage);
	SafeRelease(mBrush);
	mRenderer.Destroy();
}

void LabelCtrl::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

BOOL LabelCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void LabelCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CD2DRectF rect(clientRect);

	mRenderer.BeginDraw();
	mRenderer.Clear(mBackgroundColor);

	if (mBackgroundImage)
	{
		mRenderer.DrawBitmap(mBackgroundImage, &rect);
	}

	CString wndText;
	GetWindowText(wndText);

	IDWriteTextFormat* textFormat;
	if (mTextSize == TEST_SIZE_SMALL)
	{
		textFormat = DWrite::segoe12R;
	}
	else if (mTextSize == TEXT_SIZE_MEDIUM)
	{
		textFormat = DWrite::segoe16R;
	}
	else
	{
		textFormat = DWrite::segoe20R;
	}

	DWRITE_TEXT_ALIGNMENT textAlignment = textFormat->GetTextAlignment();
	DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = textFormat->GetParagraphAlignment();

	textFormat->SetTextAlignment(mTextAlignment);
	textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	mBrush->SetColor(mForegroundColor);
	rect.top -= 1.0f;
	rect.bottom -= 1.0f;
	rect.left += 1.0f;
	rect.right -= 1.0f;
	mRenderer.DrawText(wndText, (UINT32)wndText.GetLength(), textFormat, rect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);

	textFormat->SetTextAlignment(textAlignment);
	textFormat->SetParagraphAlignment(paragraphAlignment);

	if (mDrawBorder)
	{
		if (GetStyle() & WS_DISABLED)
		{
			mBrush->SetColor(VisualManager::CLR_BASE);
		}
		else
		{
			mBrush->SetColor(mBorderColor);
		}
		rect.left = 1;
		rect.top = 1;
		rect.right = (FLOAT)clientRect.right;
		rect.bottom = (FLOAT)clientRect.bottom;
		mRenderer.DrawRectangle(rect, mBrush);
	}

	mRenderer.EndDraw();

	mRenderer.Display();
}

} // namespace hsui
