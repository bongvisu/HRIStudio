#include "hsuiPrecompiled.h"
#include "hsuiEditCtrl.h"
#include "hsuiResource.h"
#include "hsuiVisualManager.h"

using namespace std;
using namespace xsc;

namespace hsui {

// ====================================================================================================================
HACCEL EditCtrl::accelerator;

void EditCtrl::InitEditCtrl()
{
	accelerator = LoadAcceleratorsW(hsui::Module::handle, MAKEINTRESOURCE(HSUI_IDR_EDIT_ACCEL));
}

void EditCtrl::TermEditCtrl()
{
	DestroyAcceleratorTable(accelerator);
}
// ====================================================================================================================

static const UINT WND_ID_EDIT = 1;
const int EditCtrl::FIXED_HEIGHT = 22;

BEGIN_MESSAGE_MAP(EditCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_CONTROL(EN_KILLFOCUS, WND_ID_EDIT, OnEditKillFocus)
END_MESSAGE_MAP()

EditCtrl::EditCtrl()
	:
	mActive(false),
	mBorderNormal(nullptr),
	mBorderActive(nullptr)
{
}

EditCtrl::~EditCtrl()
{
}

void EditCtrl::SetEditText(const CString& text)
{
	assert(GetSafeHwnd());
	mEdit.SetWindowText(text);
}

CString EditCtrl::GetEditText() const
{
	assert(GetSafeHwnd());
	CString text;
	mEdit.GetWindowText(text);
	return text;
}

void EditCtrl::CreateWnd(CWnd* parent, int x, int y, int w)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect(x, y, x + w, y + FIXED_HEIGHT);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, rect, parent, 0);
}

CEdit& EditCtrl::GetEditWnd()
{
	return mEdit;
}

void EditCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void EditCtrl::ResizeWnd(int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, 0, 0, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void EditCtrl::MoveAndResizeWnd(int x, int y, int w)
{
	assert(GetSafeHwnd());

	SetWindowPos(nullptr, x, y, w, FIXED_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
}

void EditCtrl::OnEditSelectAll()
{
	mEdit.SetSel(0, mEdit.GetWindowTextLengthW());
}

void EditCtrl::OnEditCopy()
{
	mEdit.Copy();
}

void EditCtrl::OnEditCut()
{
	mEdit.Cut();
}

void EditCtrl::OnEditPaste()
{
	mEdit.Paste();
}

void EditCtrl::OnEditKillFocus()
{
	mActive = false;
	Invalidate(FALSE);
}

HBRUSH EditCtrl::OnCtlColor(CDC* dc, CWnd* wnd, UINT controlID)
{
	if (controlID == WND_ID_EDIT)
	{
		dc->SetBkColor(VisualManager::ToGdiColor(VisualManager::CLR_WHITE));
		dc->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_LEVEL1));
		return static_cast<HBRUSH>(theVisualManager->GetWhiteBrush()->m_hObject);
	}
	return CWnd::OnCtlColor(dc, wnd, controlID);
}

BOOL EditCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == mEdit.GetSafeHwnd())
	{
		if (TranslateAccelerator(m_hWnd, accelerator, pMsg))
		{
			return TRUE;
		}

		if (pMsg->message == WM_MOUSEMOVE)
		{
			if (mActive == false)
			{
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof(trackMouseEvent);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = pMsg->hwnd;
				TrackMouseEvent(&trackMouseEvent);

				mActive = true;
				Invalidate(FALSE);
			}
		}
		else if (pMsg->message == WM_MOUSELEAVE)
		{
			mActive = false;
			Invalidate(FALSE);
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

int EditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	DWORD wndStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT;
	mEdit.Create(wndStyle, { 2, 3, 0, 0 }, this, WND_ID_EDIT);

	mEdit.SetFont(theVisualManager->GetCommonFontR(), FALSE);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBorderNormal);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_DARK_BLUE, &mBorderActive);

	return 0;
}

void EditCtrl::OnDestroy()
{
	SafeRelease(mBorderNormal);
	SafeRelease(mBorderActive);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void EditCtrl::OnSize(UINT type, int cx, int cy)
{
	DWORD swpFlags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE;
	mEdit.SetWindowPos(nullptr, 0, 0, cx - 4, cy - 4, swpFlags);

	mRenderer.Resize(cx, cy);
}

BOOL EditCtrl::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void EditCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);

	D2D1_POINT_2F pt0, pt1;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_WHITE);

	ID2D1SolidColorBrush* borderBrush;
	if (mActive || (GetFocus() == &mEdit))
	{
		borderBrush = mBorderActive;
	}
	else
	{
		borderBrush = mBorderNormal;
	}

	pt0.x = 0.0f;
	pt0.y = 1.0f;
	pt1.x = (FLOAT)clientRect.right;
	pt1.y = pt0.y;
	mRenderer.DrawLine(pt0, pt1, borderBrush);

	pt0.x = (FLOAT)clientRect.right;
	pt0.y = 1.0f;
	pt1.x = pt0.x;
	pt1.y = (FLOAT)clientRect.bottom;
	mRenderer.DrawLine(pt0, pt1, borderBrush);

	pt0.x = 0.0f;
	pt0.y = (FLOAT)clientRect.bottom;
	pt1.x = (FLOAT)clientRect.right;
	pt1.y = pt0.y;
	mRenderer.DrawLine(pt0, pt1, borderBrush);

	pt0.x = 1.0f;
	pt0.y = 0.0f;
	pt1.x = pt0.x;
	pt1.y = (FLOAT)clientRect.bottom;
	mRenderer.DrawLine(pt0, pt1, borderBrush);

	mRenderer.EndDraw();

	mRenderer.Display();
}

} // namespace mco
