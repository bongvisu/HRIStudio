#include "hsuiPrecompiled.h"
#include "hsuiSystemButtons.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>

using namespace std;
using namespace xsc;

namespace hsui {

const int SystemButton::FIXED_BUTTON_WIDTH = 34;
const int SystemButton::FIXED_BUTTON_HEIGHT = 26;

BEGIN_MESSAGE_MAP(SystemButton, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SystemButton::SystemButton()
{
	mToolTip = nullptr;
}

SystemButton::~SystemButton()
{
}

BOOL SystemButton::PreTranslateMessage(MSG* pMsg)
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

void SystemButton::CreateWnd(CWnd* parent)
{
	assert(GetSafeHwnd() == nullptr);

	Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 8, 8), parent, 0);
}

void SystemButton::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	UINT swpFlags = SWP_NOZORDER | SWP_NOACTIVATE;
	SetWindowPos(nullptr, x, y, FIXED_BUTTON_WIDTH, FIXED_BUTTON_HEIGHT, swpFlags);
}

int SystemButton::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	CTooltipManager::CreateToolTip(mToolTip, this, AFX_TOOLTIP_TYPE_BUTTON);
	mToolTip->Activate(TRUE);

	return 0;
}

void SystemButton::OnDestroy()
{
	CTooltipManager::DeleteToolTip(mToolTip);
	mToolTip = nullptr;

	CWnd::OnDestroy();
}

BEGIN_MESSAGE_MAP(SystemButtonMinimize, SystemButton)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()

	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

SystemButtonMinimize::SystemButtonMinimize()
{
	mImages[BUTTON_STATE_NORMAL ] = nullptr;
	mImages[BUTTON_STATE_HOVERED] = nullptr;
	mImages[BUTTON_STATE_PRESSED] = nullptr;

	mButtonState = BUTTON_STATE_NORMAL;
}

SystemButtonMinimize::~SystemButtonMinimize()
{
}

int SystemButtonMinimize::OnCreate(LPCREATESTRUCT createStruct)
{
	SystemButton::OnCreate(createStruct);

	mToolTip->AddTool(this, L"Minimize");

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	
	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MIN_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_NORMAL]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MIN_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_HOVERED]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MIN_P);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_PRESSED]));

	return 0;
}

void SystemButtonMinimize::OnDestroy()
{
	for (int i = 0; i < NUMBER_OF_BUTTON_STATES; ++i)
	{
		mImages[i]->Release();
	}
	mRenderer.Destroy();

	SystemButton::OnDestroy();
}

void SystemButtonMinimize::OnSize(UINT type, int cx, int cy)
{
	SystemButton::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);
}

void SystemButtonMinimize::OnMouseMove(UINT flags, CPoint point)
{
	SystemButton::OnMouseMove(flags, point);

	if (SystemButton::GetCapture() != this)
	{
		if (mButtonState != BUTTON_STATE_HOVERED)
		{
			mButtonState = BUTTON_STATE_HOVERED;
			Invalidate(FALSE);

			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize = sizeof(trackmouseevent);
			trackmouseevent.dwFlags = TME_LEAVE;
			trackmouseevent.hwndTrack = m_hWnd;
			TrackMouseEvent(&trackmouseevent);
		}
	}

	mMousePosition = point;
}

void SystemButtonMinimize::OnMouseLeave()
{
	SystemButton::OnMouseLeave();

	mButtonState = BUTTON_STATE_NORMAL;
	Invalidate(FALSE);
}

void SystemButtonMinimize::OnLButtonDown(UINT flags, CPoint point)
{
	SystemButton::OnLButtonDown(flags, point);

	mButtonState = BUTTON_STATE_PRESSED;

	SetCapture();

	Invalidate(FALSE);
}

void SystemButtonMinimize::OnLButtonUp(UINT flags, CPoint point)
{
	SystemButton::OnLButtonUp(flags, point);

	ReleaseCapture();
}

void SystemButtonMinimize::OnCaptureChanged(CWnd* wnd)
{
	SystemButton::OnCaptureChanged(wnd);

	if (wnd != this)
	{
		if (mButtonState == BUTTON_STATE_PRESSED)
		{
			CRect rect;
			GetClientRect(&rect);
			if (rect.PtInRect(mMousePosition))
			{
				mButtonState = BUTTON_STATE_HOVERED;
				GetParent()->PostMessageW(WM_SYSCOMMAND, SC_MINIMIZE);
			}
			else
			{
				mButtonState = BUTTON_STATE_NORMAL;
			}

			Invalidate(FALSE);
		}
	}
}

BOOL SystemButtonMinimize::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void SystemButtonMinimize::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();

	mRenderer.DrawBitmap(mImages[mButtonState]);

	mRenderer.EndDraw();

	mRenderer.Display();
}

BEGIN_MESSAGE_MAP(SystemButtonMaxRestore, SystemButton)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()

	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

SystemButtonMaxRestore::SystemButtonMaxRestore()
{
	for (int i = 0; i < NUMBER_OF_BUTTON_STATES; ++i)
	{
		mMaxImages[i] = nullptr;
		mResImages[i] = nullptr;
	}

	mMaximized = false;

	mButtonState = BUTTON_STATE_NORMAL;
}

SystemButtonMaxRestore::~SystemButtonMaxRestore()
{
}

int SystemButtonMaxRestore::OnCreate(LPCREATESTRUCT createStruct)
{
	SystemButton::OnCreate(createStruct);

	DWORD mainFrameStyle = ::AfxGetApp()->m_pMainWnd->GetStyle();
	if (mainFrameStyle & WS_MAXIMIZE)
	{
		mMaximized = true;
		mToolTip->AddTool(this, L"Restore");
	}
	else
	{
		mMaximized = false;
		mToolTip->AddTool(this, L"Maximize");
	}

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);

	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MAX_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mMaxImages[BUTTON_STATE_NORMAL]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MAX_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mMaxImages[BUTTON_STATE_HOVERED]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_MAX_P);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mMaxImages[BUTTON_STATE_PRESSED]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_RESTORE_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mResImages[BUTTON_STATE_NORMAL]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_RESTORE_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mResImages[BUTTON_STATE_HOVERED]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_RESTORE_P);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mResImages[BUTTON_STATE_PRESSED]));

	return 0;
}

void SystemButtonMaxRestore::OnDestroy()
{
	for (int i = 0; i < NUMBER_OF_BUTTON_STATES; ++i)
	{
		SafeRelease(mMaxImages[i]);
		SafeRelease(mResImages[i]);
	}
	mRenderer.Destroy();

	SystemButton::OnDestroy();
}

void SystemButtonMaxRestore::OnSize(UINT type, int cx, int cy)
{
	SystemButton::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);
}

void SystemButtonMaxRestore::OnMouseMove(UINT flags, CPoint point)
{
	SystemButton::OnMouseMove(flags, point);

	if (CWnd::GetCapture() != this)
	{
		if (mButtonState != BUTTON_STATE_HOVERED)
		{
			mButtonState = BUTTON_STATE_HOVERED;
			Invalidate(FALSE);

			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize = sizeof(trackmouseevent);
			trackmouseevent.dwFlags = TME_LEAVE;
			trackmouseevent.hwndTrack = m_hWnd;
			TrackMouseEvent(&trackmouseevent);
		}
	}

	mMousePosition = point;
}

void SystemButtonMaxRestore::OnMouseLeave()
{
	SystemButton::OnMouseLeave();

	mButtonState = BUTTON_STATE_NORMAL;
	Invalidate(FALSE);
}

void SystemButtonMaxRestore::OnLButtonDown(UINT flags, CPoint point)
{
	SystemButton::OnLButtonDown(flags, point);

	mButtonState = BUTTON_STATE_PRESSED;

	SetCapture();

	Invalidate(FALSE);
}

void SystemButtonMaxRestore::OnLButtonUp(UINT flags, CPoint point)
{
	SystemButton::OnLButtonUp(flags, point);

	ReleaseCapture();
}

void SystemButtonMaxRestore::OnCaptureChanged(CWnd* wnd)
{
	SystemButton::OnCaptureChanged(wnd);

	if (wnd != this)
	{
		if (mButtonState == BUTTON_STATE_PRESSED)
		{
			CRect rect;
			GetClientRect(&rect);
			if (rect.PtInRect(mMousePosition))
			{
				mButtonState = BUTTON_STATE_HOVERED;

				DWORD mainFrameStyle = ::AfxGetApp()->m_pMainWnd->GetStyle();
				if (mainFrameStyle & WS_MAXIMIZE)
				{
					GetParent()->PostMessageW(WM_SYSCOMMAND, SC_RESTORE);
				}
				else
				{
					GetParent()->PostMessageW(WM_SYSCOMMAND, SC_MAXIMIZE);
				}
			}
			else
			{
				mButtonState = BUTTON_STATE_NORMAL;
			}

			Invalidate(FALSE);
		}
	}
}

BOOL SystemButtonMaxRestore::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void SystemButtonMaxRestore::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();

	DWORD mainFrameStyle = ::AfxGetApp()->m_pMainWnd->GetStyle();
	if (mainFrameStyle & WS_MAXIMIZE)
	{
		mRenderer.DrawBitmap(mResImages[mButtonState]);

		if (!mMaximized)
		{
			mMaximized = true;
			mToolTip->UpdateTipText(L"Restore", this);
		}
	}
	else
	{
		mRenderer.DrawBitmap(mMaxImages[mButtonState]);

		if (mMaximized)
		{
			mMaximized = false;
			mToolTip->UpdateTipText(L"Maximize", this);
		}
	}

	mRenderer.EndDraw();

	mRenderer.Display();
}

BEGIN_MESSAGE_MAP(SystemButtonClose, SystemButton)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()

	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

SystemButtonClose::SystemButtonClose()
{
	mButtonState = BUTTON_STATE_NORMAL;

	mImages[BUTTON_STATE_NORMAL] = nullptr;
	mImages[BUTTON_STATE_HOVERED] = nullptr;
	mImages[BUTTON_STATE_PRESSED] = nullptr;
}

SystemButtonClose::~SystemButtonClose()
{
}

int SystemButtonClose::OnCreate(LPCREATESTRUCT createStruct)
{
	SystemButton::OnCreate(createStruct);

	mToolTip->AddTool(this, L"Close");

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);

	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_CLOSE_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_NORMAL]));
	
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_CLOSE_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_HOVERED]));

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SYS_BUTTON_CLOSE_P);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &(mImages[BUTTON_STATE_PRESSED]));

	return 0;
}

void SystemButtonClose::OnDestroy()
{
	for (int i = 0; i < NUMBER_OF_BUTTON_STATES; ++i)
	{
		SafeRelease(mImages[i]);
	}
	mRenderer.Destroy();

	SystemButton::OnDestroy();
}

void SystemButtonClose::OnSize(UINT type, int cx, int cy)
{
	SystemButton::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);
}

void SystemButtonClose::OnMouseMove(UINT flags, CPoint point)
{
	SystemButton::OnMouseMove(flags, point);

	if (CWnd::GetCapture() != this)
	{
		if (mButtonState != BUTTON_STATE_HOVERED)
		{
			mButtonState = BUTTON_STATE_HOVERED;
			Invalidate(FALSE);

			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize = sizeof(trackmouseevent);
			trackmouseevent.dwFlags = TME_LEAVE;
			trackmouseevent.hwndTrack = m_hWnd;
			TrackMouseEvent(&trackmouseevent);
		}
	}

	mMousePosition = point;
}

void SystemButtonClose::OnMouseLeave()
{
	SystemButton::OnMouseLeave();

	mButtonState = BUTTON_STATE_NORMAL;
	Invalidate(FALSE);
}

void SystemButtonClose::OnLButtonDown(UINT flags, CPoint point)
{
	SystemButton::OnLButtonDown(flags, point);

	mButtonState = BUTTON_STATE_PRESSED;

	SetCapture();

	Invalidate(FALSE);
}

void SystemButtonClose::OnLButtonUp(UINT flags, CPoint point)
{
	SystemButton::OnLButtonUp(flags, point);

	ReleaseCapture();
}

void SystemButtonClose::OnCaptureChanged(CWnd* wnd)
{
	SystemButton::OnCaptureChanged(wnd);

	if (wnd != this)
	{
		if (mButtonState == BUTTON_STATE_PRESSED)
		{
			CRect rect;
			GetClientRect(&rect);
			if (rect.PtInRect(mMousePosition))
			{
				mButtonState = BUTTON_STATE_HOVERED;
				GetParent()->PostMessageW(WM_SYSCOMMAND, SC_CLOSE);
			}
			else
			{
				mButtonState = BUTTON_STATE_NORMAL;
			}

			Invalidate(FALSE);
		}
	}
}

BOOL SystemButtonClose::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void SystemButtonClose::OnPaint()
{
	CPaintDC dc(this);
	mRenderer.BeginDraw();

	mRenderer.DrawBitmap(mImages[mButtonState]);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsui
