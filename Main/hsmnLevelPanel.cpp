#include "hsmnPrecompiled.h"
#include "hsmnLevelPanel.h"
#include <xscDWrite.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(LevelPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	
	ON_WM_SETFOCUS()

	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LevelPanel::LevelPanel(xsc::Level* level, CWnd* parent)
	:
	mLevel(level)
{
	CRect rect(0, 0, 8, 8);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, rect, parent, AFX_IDW_PANE_FIRST, nullptr);
}

LevelPanel::~LevelPanel()
{
}

void LevelPanel::OnSetFocus(CWnd* oldWnd)
{
	if (mView)
	{
		mView->SetFocus();
	}
	else
	{
		CWnd::OnSetFocus(oldWnd);
	}
}

void LevelPanel::SetHorzRange(int start, int end)
{
	mHorzScrollBar.SetRange(start, end);
}

void LevelPanel::SetVertRange(int start, int end)
{
	mVertScrollBar.SetRange(start, end);
}

int LevelPanel::GetHorzOffset() const
{
	return mHorzScrollBar.GetScrollPosition();
}

int LevelPanel::GetVertOffset() const
{
	return mVertScrollBar.GetScrollPosition();
}

void LevelPanel::UpdatePointerInfo(int xWorld, int yWorld)
{
	mCoordinateGadget.SetPointerInfo(xWorld, yWorld);
}

int LevelPanel::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);
	
	mHorzScrollBar.CreateWnd(this, 0, 0, 100);
	mVertScrollBar.CreateWnd(this, 0, 0, 100);

	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	CRect rect(0, 0, 8, 8);
	mCoordinateGadget.Create(nullptr, nullptr, style, rect, this, 0);

	mView = make_unique<LevelView>(mLevel, this);

	return 0;
}

void LevelPanel::OnDestroy()
{
	mView->DestroyWindow();
	mView.reset();

	CWnd::OnDestroy();
}

void LevelPanel::OnSize(UINT type, int cx, int cy)
{
	if (nullptr == mView->GetSafeHwnd())
	{
		return;
	}
	
	HDWP hdwp = BeginDeferWindowPos(4);

	int x, y, w, h;

	x = 1;
	y = 1;
	w = cx - ScrollBar::FIXED_SIZE - 2;
	h = cy - ScrollBar::FIXED_SIZE - 2;
	DeferWindowPos(hdwp, mView->m_hWnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	x = cx - ScrollBar::FIXED_SIZE - 1;
	y = 1;
	w = ScrollBar::FIXED_SIZE;
	h = cy - ScrollBar::FIXED_SIZE - 2;
	DeferWindowPos(hdwp, mVertScrollBar.m_hWnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	static const int COORD_GADGET_SIZE = 110;

	x = 1;
	y = cy - ScrollBar::FIXED_SIZE - 1;
	w = cx - COORD_GADGET_SIZE - 2;
	h = ScrollBar::FIXED_SIZE;
	DeferWindowPos(hdwp, mHorzScrollBar.m_hWnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	x = cx - COORD_GADGET_SIZE - 1;
	y = cy - ScrollBar::FIXED_SIZE - 1;
	w = COORD_GADGET_SIZE;
	h = ScrollBar::FIXED_SIZE;
	DeferWindowPos(hdwp, mCoordinateGadget.m_hWnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	EndDeferWindowPos(hdwp);
}

BOOL LevelPanel::OnCmdMsg(UINT id, int code, void* extra, AFX_CMDHANDLERINFO* handlerInfo)
{
	if (mView->OnCmdMsg(id, code, extra, handlerInfo))
	{
		return TRUE;
	}
	return CWnd::OnCmdMsg(id, code, extra, handlerInfo);
}

BOOL LevelPanel::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void LevelPanel::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	auto borderColor = VisualManager::ToGdiColor(VisualManager::CLR_LEVEL3);
	dc.Draw3dRect(&rect, borderColor, borderColor);
}

BEGIN_MESSAGE_MAP(CoordinateGadget, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CoordinateGadget::CoordinateGadget()
{
	mWorldX = mWorldY = 0;
}

CoordinateGadget::~CoordinateGadget()
{
}

void CoordinateGadget::SetPointerInfo(int worldX, int worldY)
{
	mWorldX = worldX;
	mWorldY = worldY;
	Invalidate(FALSE);
}

int CoordinateGadget::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, 8, 8);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrush);

	return 0;
}

void CoordinateGadget::OnDestroy()
{
	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void CoordinateGadget::OnSize(UINT type, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

BOOL CoordinateGadget::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void CoordinateGadget::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	CRect clientRect;
	GetClientRect(&clientRect);

	D2D1_RECT_F rect;
	rect.left = clientRect.left + 1.0f;
	rect.top = clientRect.top + 1.0f;
	rect.right = static_cast<FLOAT>(clientRect.right);
	rect.bottom = static_cast<FLOAT>(clientRect.bottom);

	CString pointerInfo;
	pointerInfo.Format(L"X : %d, Y : %d", mWorldX, mWorldY);
	mBrush->SetColor(VisualManager::CLR_LEVEL1);

	D2D1_RECT_F textRect = rect;
	textRect.top -= 1;
	textRect.bottom -= 1;
	IDWriteTextFormat* font = DWrite::segoe12R;
	font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mRenderer.DrawText(pointerInfo, (UINT32)pointerInfo.GetLength(), font, textRect, mBrush);
	font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
