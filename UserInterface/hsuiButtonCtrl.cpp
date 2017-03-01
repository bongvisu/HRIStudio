#include "hsuiPrecompiled.h"
#include "hsuiButtonCtrl.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscDWrite.h>

using namespace xsc;
using namespace hsmo;

namespace hsui {

BEGIN_MESSAGE_MAP(ButtonCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ButtonCtrl::ButtonCtrl()
{
	mListener = nullptr;
	
	mToolTip = nullptr;

	mMode = MODE_IMAGE;

	std::fill(mImages, mImages + NUMBER_OF_STATES, nullptr);
	mDisabledImage = nullptr;

	mBackgroundColor = VisualManager::CLR_BASE;
	mBackgroundImage = nullptr;

	mBrush = nullptr;
	mGradientBrushes[STATE_NORMAL]	= nullptr;
	mGradientBrushes[STATE_HOT]		= nullptr;
	mGradientBrushes[STATE_PRESSED]	= nullptr;

	mRounded = true;
	mOuterColors [STATE_NORMAL]		= VisualManager::CLR_DARK_BASE;
	mOuterColors [STATE_HOT]		= VisualManager::CLR_BLUE;
	mOuterColors [STATE_PRESSED]	= VisualManager::CLR_DARK_BLUE;
	mCenterColors[STATE_NORMAL]		= VisualManager::CLR_DARK_BASE;
	mCenterColors[STATE_HOT]		= VisualManager::CLR_BLUE;
	mCenterColors[STATE_PRESSED]	= VisualManager::CLR_DARK_BLUE;
	mTextColors  [STATE_NORMAL]		= VisualManager::CLR_LEVEL1;
	mTextColors  [STATE_HOT]		= VisualManager::CLR_WHITE;
	mTextColors  [STATE_PRESSED]	= VisualManager::CLR_WHITE;

	mButtonState = STATE_NORMAL;
}

ButtonCtrl::~ButtonCtrl()
{
}

void ButtonCtrl::SetListener(ButtonCtrl::IListener* listener)
{
	mListener = listener;
}

void ButtonCtrl::ResetListener()
{
	mListener = nullptr;
}

void ButtonCtrl::SetMode(Mode mode)
{
	if (mMode == mode)
	{
		return; // the same as the current mode
	}
	mMode = mode;

	if (nullptr == GetSafeHwnd())
	{
		return; // the window not created yet
	}

	if (mode == MODE_TEXT)
	{
		mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrush);

		SetupGradientBrush(STATE_NORMAL);
		SetupGradientBrush(STATE_HOT);
		SetupGradientBrush(STATE_PRESSED);
	}
	else
	{
		SafeRelease(mBrush);
		SafeRelease(mGradientBrushes[STATE_NORMAL]);
		SafeRelease(mGradientBrushes[STATE_HOT]);
		SafeRelease(mGradientBrushes[STATE_PRESSED]);
	}

	Invalidate(FALSE);
}

void ButtonCtrl::SetRounded(bool rounded)
{
	if (rounded == mRounded) return;

	mRounded = rounded;

	if (GetSafeHwnd() && mMode == MODE_TEXT)
	{
		Invalidate(FALSE);
	}
}

void ButtonCtrl::SetNormalColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center)
{
	mOuterColors [STATE_NORMAL] = outer;
	mCenterColors[STATE_NORMAL] = center;
	if (GetSafeHwnd() && mMode == MODE_TEXT)
	{
		SetupGradientBrush(STATE_NORMAL);
		Invalidate(FALSE);
	}
}

void ButtonCtrl::SetHotColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center)
{
	mOuterColors [STATE_HOT] = outer;
	mCenterColors[STATE_HOT] = center;
	if (GetSafeHwnd() && mMode == MODE_TEXT)
	{
		SetupGradientBrush(STATE_HOT);
		Invalidate(FALSE);
	}
}

void ButtonCtrl::SetPressedColors(const D2D1_COLOR_F& outer, const D2D1_COLOR_F& center)
{
	mOuterColors [STATE_PRESSED] = outer;
	mCenterColors[STATE_PRESSED] = center;
	if (GetSafeHwnd() && mMode == MODE_TEXT)
	{
		SetupGradientBrush(STATE_PRESSED);
		Invalidate(FALSE);
	}
}

void ButtonCtrl::SetTextColor(const D2D1_COLOR_F& normal, const D2D1_COLOR_F& hot, const D2D1_COLOR_F& pressed)
{
	mTextColors[STATE_NORMAL]	= normal;
	mTextColors[STATE_HOT]		= hot;
	mTextColors[STATE_PRESSED]	= pressed;
	if (GetSafeHwnd() && mMode == MODE_TEXT)
	{
		Invalidate(FALSE);
	}
}

void ButtonCtrl::CreateWnd(CWnd* parent, const CString& text, int x, int y, int w, int h)
{
	assert(GetSafeHwnd() == nullptr);
	
	CRect rect(x, y, x + w, y + h);
	Create(nullptr, text, WS_CHILD | WS_VISIBLE, rect, parent, 0);
}

void ButtonCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ButtonCtrl::ResizeWnd(int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void ButtonCtrl::MoveAndResizeWnd(int x, int y, int w, int h)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL ButtonCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (mToolTip)
	{
		if (pMsg->message == WM_LBUTTONDOWN ||
			pMsg->message == WM_LBUTTONUP ||
			pMsg->message == WM_MOUSEMOVE)
		{
			mToolTip->RelayEvent(pMsg);
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void ButtonCtrl::SetupGradientBrush(State state)
{
	SafeRelease(mGradientBrushes[state]);

	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES prop;
	prop.startPoint.x = 0.0f;
	prop.startPoint.y = 0.0f;
	prop.endPoint.x = 0.0f;
	prop.endPoint.y = 0.5f;

	D2D1_GRADIENT_STOP stops[2];
	ID2D1GradientStopCollection* stopCollection = nullptr;
	stops[0].position = 0.0f;
	stops[1].position = 1.0f;

	stops[0].color = mOuterColors [state];
	stops[1].color = mCenterColors[state];
	mRenderer.CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_MIRROR, &stopCollection);

	mRenderer.CreateLinearGradientBrush(prop, stopCollection, &mGradientBrushes[state]);
	stopCollection->Release();
}

int ButtonCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	
	if (mMode == MODE_TEXT)
	{
		mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrush);
		SetupGradientBrush(STATE_NORMAL);
		SetupGradientBrush(STATE_HOT);
		SetupGradientBrush(STATE_PRESSED);
	}

	CTooltipManager::CreateToolTip(mToolTip, this, AFX_TOOLTIP_TYPE_BUTTON);
	mToolTip->Activate(TRUE);

	return 0;
}

void ButtonCtrl::OnDestroy()
{
	CTooltipManager::DeleteToolTip(mToolTip);
	mToolTip = nullptr;

	for (int i = 0; i < NUMBER_OF_STATES; ++i)
	{
		SafeRelease(mImages[i]);
		SafeRelease(mGradientBrushes[i]);
	}
	SafeRelease(mDisabledImage);
	SafeRelease(mBackgroundImage);
	SafeRelease(mBrush);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ButtonCtrl::SetBackgroundImage(const WicBitmap& image)
{
	assert(GetSafeHwnd());

	SafeRelease(mBackgroundImage);

	mRenderer.CreateBitmapFromWicBitmap(image.GetSource(), &mBackgroundImage);
}

void ButtonCtrl::SetBackgroundColor(const D2D1_COLOR_F& color)
{
	mBackgroundColor = color;
	if (GetSafeHwnd()) Invalidate(FALSE);
}

void ButtonCtrl::SetNormalImage(const WicBitmap& image)
{
	assert(GetSafeHwnd());

	SafeRelease(mImages[STATE_NORMAL]);

	mRenderer.CreateBitmapFromWicBitmap(image.GetSource(), &(mImages[STATE_NORMAL]));

	Invalidate(FALSE);
}

void ButtonCtrl::SetHotImage(const WicBitmap& image)
{
	assert(GetSafeHwnd());

	SafeRelease(mImages[STATE_HOT]);

	mRenderer.CreateBitmapFromWicBitmap(image.GetSource(), &(mImages[STATE_HOT]));

	Invalidate(FALSE);
}

void ButtonCtrl::SetPressedImage(const WicBitmap& image)
{
	assert(GetSafeHwnd());

	SafeRelease(mImages[STATE_PRESSED]);

	mRenderer.CreateBitmapFromWicBitmap(image.GetSource(), &(mImages[STATE_PRESSED]));

	Invalidate(FALSE);
}

void ButtonCtrl::SetDisabledImage(const WicBitmap& image)
{
	assert(GetSafeHwnd());

	SafeRelease(mDisabledImage);

	mRenderer.CreateBitmapFromWicBitmap(image.GetSource(), &mDisabledImage);

	Invalidate(FALSE);
}

void ButtonCtrl::ActivateToolTip()
{
	CString text;
	GetWindowText(text);
	mToolTip->AddTool(this, text);
}

void ButtonCtrl::DeactivateToolTip()
{
	mToolTip->DelTool(this);
}

void ButtonCtrl::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

void ButtonCtrl::OnEnable(BOOL bEnable)
{
	Invalidate(FALSE);
}

void ButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (CWnd::GetCapture() != this)
	{
		if (mButtonState != STATE_HOT)
		{
			mButtonState = STATE_HOT;
			Invalidate(FALSE);

			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof(trackMouseEvent);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = m_hWnd;
			TrackMouseEvent(&trackMouseEvent);
		}
	}

	mMousePosition = point;
}

void ButtonCtrl::OnMouseLeave()
{
	mButtonState = STATE_NORMAL;
	Invalidate(FALSE);
}

void ButtonCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	mButtonState = STATE_PRESSED;
	Invalidate(FALSE);
}

void ButtonCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (mButtonState == STATE_PRESSED)
	{
		ReleaseCapture();

		CRect rect;
		GetClientRect(&rect);
		if (rect.PtInRect(mMousePosition))
		{
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof(trackMouseEvent);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = m_hWnd;
			TrackMouseEvent(&trackMouseEvent);

			mButtonState = STATE_HOT;
			Invalidate(FALSE);

			if (mListener) mListener->OnClicked(this);
		}
		else
		{
			mButtonState = STATE_NORMAL;
			Invalidate(FALSE);
		}
	}
}

BOOL ButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void ButtonCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);

	mRenderer.BeginDraw();
	
	if (mBackgroundImage)
	{
		CD2DRectF rect(clientRect);
		mRenderer.DrawBitmap(mBackgroundImage, rect);
	}
	else
	{
		mRenderer.Clear(mBackgroundColor);
	}

	if (mMode == MODE_IMAGE)
	{
		RenderImageMode(clientRect);
	}
	else
	{
		RenderTextMode(clientRect);
	}
	
	mRenderer.EndDraw();

	mRenderer.Display();
}

void ButtonCtrl::RenderImageMode(CRect clientRect)
{
	D2D1_RECT_F rect;

	if (GetStyle() & WS_DISABLED)
	{
		if (mDisabledImage)
		{
			D2D1_SIZE_U imageSize = mDisabledImage->GetPixelSize();
			rect.left = (clientRect.Width() - ((int)imageSize.width)) * 0.5f;
			rect.top = (FLOAT)((clientRect.bottom - (int)imageSize.height) / 2);
			rect.right = rect.left + imageSize.width;
			rect.bottom = rect.top + imageSize.height;
			mRenderer.DrawBitmap(mDisabledImage, rect);
		}
	}
	else
	{
		if (mImages[mButtonState])
		{
			D2D1_SIZE_U imageSize = mImages[mButtonState]->GetPixelSize();
			rect.left = (clientRect.Width() - ((int)imageSize.width)) * 0.5f;
			rect.top = static_cast<FLOAT>((clientRect.bottom - (int)imageSize.height) / 2);
			rect.right = rect.left + imageSize.width;
			rect.bottom = rect.top + imageSize.height;
			mRenderer.DrawBitmap(mImages[mButtonState], rect);
		}
	}
}

void ButtonCtrl::RenderTextMode(CRect clientRect)
{
	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(clientRect.left);
	rect.top = static_cast<FLOAT>(clientRect.top);
	rect.right = static_cast<FLOAT>(clientRect.right);
	rect.bottom = static_cast<FLOAT>(clientRect.bottom);

	CString text;
	GetWindowTextW(text);

	if (GetStyle() & WS_DISABLED)
	{
		D2D1_POINT_2F point;
		point.x = 0.0f;
		point.y = 0.0f;
		mGradientBrushes[mButtonState]->SetStartPoint(point);
		point.y = rect.bottom * 0.5f;
		mGradientBrushes[mButtonState]->SetEndPoint(point);

		if (mRounded)
		{
			static const FLOAT CORNER_RADIUS = 7.0f;
			D2D1_ROUNDED_RECT roundedRect;
			roundedRect.rect = rect;
			roundedRect.radiusX = CORNER_RADIUS;
			roundedRect.radiusY = CORNER_RADIUS;
			mRenderer.FillRoundedRectangle(roundedRect, mGradientBrushes[mButtonState]);
		}
		else
		{
			mRenderer.FillRectangle(rect, mGradientBrushes[mButtonState]);
		}

		if (false == mRounded)
		{
			mBrush->SetColor(mCenterColors[mButtonState]);
			mRenderer.DrawRectangle(rect, mBrush);
		}

		mBrush->SetColor(VisualManager::CLR_LEVEL2);
		rect.top -= 1.0f;
		rect.bottom -= 1.0f;
		IDWriteTextFormat* textFormat = DWrite::segoe12R;
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		mRenderer.DrawText(text, text.GetLength(), textFormat, rect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
	else
	{
		D2D1_POINT_2F point;
		point.x = 0.0f;
		point.y = 0.0f;
		mGradientBrushes[mButtonState]->SetStartPoint(point);
		point.y = rect.bottom * 0.5f;
		mGradientBrushes[mButtonState]->SetEndPoint(point);

		if (mRounded)
		{
			static const FLOAT CORNER_RADIUS = 7.0f;
			D2D1_ROUNDED_RECT roundedRect;
			roundedRect.rect = rect;
			roundedRect.radiusX = CORNER_RADIUS;
			roundedRect.radiusY = CORNER_RADIUS;
			mRenderer.FillRoundedRectangle(roundedRect, mGradientBrushes[mButtonState]);
		}
		else
		{
			mRenderer.FillRectangle(rect, mGradientBrushes[mButtonState]);
		}

		if (false == mRounded)
		{
			mBrush->SetColor(mCenterColors[mButtonState]);
			mRenderer.DrawRectangle(rect, mBrush);
		}

		mBrush->SetColor(mTextColors[mButtonState]);
		rect.top -= 1.0f;
		rect.bottom -= 1.0f;
		IDWriteTextFormat* textFormat = DWrite::segoe12R;
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		mRenderer.DrawText(text, text.GetLength(), textFormat, rect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
		textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
}

} // namespace hsui
