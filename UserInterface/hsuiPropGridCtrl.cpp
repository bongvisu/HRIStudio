#include "hsuiPrecompiled.h"
#include "afxtagmanager.h"
#include "hsuiPropGridCtrl.h"
#include "hsuiPropGridColorProperty.h"
#include "hsuiPropGridFileProperty.h"
#include "hsuiPropGridFontProperty.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"

#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

#undef max
#undef min

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

static const UINT WND_ID_HEADER	= 1;
static const UINT WND_ID_SCROLL_VERT = 2;

static const int PGC_STRETCH_DELTA = 2;
static const int PGC_TEXT_MARGIN = 4;

static const UINT PGC_WM_UPDATESPIN	= WM_USER + 101;

IMPLEMENT_DYNAMIC(PropGridCtrl, CWnd)

BEGIN_MESSAGE_MAP(PropGridCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_CANCELMODE()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_STYLECHANGED()
	ON_WM_SETFONT()
	ON_WM_GETFONT()
	ON_MESSAGE(PGC_WM_UPDATESPIN, &PropGridCtrl::OnUpdateSpin)
	ON_MESSAGE(WM_GETOBJECT, &PropGridCtrl::OnGetObject)
	ON_MESSAGE(WM_MFC_INITCTRL, &PropGridCtrl::OnInitControl)
	ON_NOTIFY(HDN_ITEMCHANGED, WND_ID_HEADER, &PropGridCtrl::OnHeaderItemChanged)
	ON_NOTIFY(HDN_TRACK, WND_ID_HEADER, &PropGridCtrl::OnHeaderTrack)
	ON_NOTIFY(HDN_ENDTRACK, WND_ID_HEADER, &PropGridCtrl::OnHeaderEndTrack)
	ON_NOTIFY(UDN_DELTAPOS, WND_ID_INPLACE, &PropGridCtrl::OnSpinDeltaPos)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, &PropGridCtrl::OnNeedTipText)
	ON_EN_KILLFOCUS(WND_ID_INPLACE, &PropGridCtrl::OnEditKillFocus)
	ON_CBN_SELENDOK(WND_ID_INPLACE, &PropGridCtrl::OnSelectCombo)
	ON_CBN_CLOSEUP(WND_ID_INPLACE, &PropGridCtrl::OnCloseCombo)
	ON_CBN_KILLFOCUS(WND_ID_INPLACE, &PropGridCtrl::OnComboKillFocus)
	ON_WM_PRINTCLIENT()
END_MESSAGE_MAP()

PropGridCtrl::PropGridCtrl()
{
	mFont = nullptr;
	mEditLeftMargin = 0;
	mBoldEditLeftMargin = 0;
	mEnableHeaderCtrl = FALSE;
	mEnableDescriptionArea = FALSE;
	mDescrHeight = -1;
	mDescrRows = 3;
	mAlphabeticMode = FALSE;
	mVSDotNetLook = FALSE;
	mRectList.SetRectEmpty();
	mLeftColumnWidth = 0;
	mRectTrackHeader.SetRectEmpty();
	mRectTrackDescr.SetRectEmpty();
	mRowHeight = 0;
	mHeaderHeight = 0;
	mVertScrollOffset = 0;
	mVertScrollTotal = 0;
	mVertScrollPage = 0;
	mSelection = nullptr;
	mFocused = FALSE;
	mTooltipsCount = 0;
	mAlwaysShowUserTT = TRUE;
	mTracking = FALSE;
	mTrackingDescr = FALSE;

	mTrueString = _T("True");
	mFalseString = _T("False");

	mListDelimeter = _T(',');

	mUseControlBarColors = FALSE;
	mGroupNameFullWidth = TRUE;
	mShowDragContext = TRUE;

	mBackgroundColor = (COLORREF)-1;
	mTextColor = (COLORREF)-1;
	mGroupBackgroundColor = (COLORREF)-1;
	mGroupTextColor = (COLORREF)-1;
	mDescriptionBackgroundColor = (COLORREF)-1;
	mDescriptionTextColor = (COLORREF)-1;
	mLineColor = (COLORREF)-1;

	mMarkModifiedProperties = FALSE;

	accProp = nullptr;
	EnableActiveAccessibility();

	comboDropdownButtonImageN = nullptr;
	comboDropdownButtonImageP = nullptr;
}

PropGridCtrl::~PropGridCtrl()
{
}

void PropGridCtrl::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == nullptr)
	{
		Init();
	}
}

int PropGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	WicBitmap wicBitmap;
	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_PROP_GRID_DROPDOWN_BUTTON_N);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &comboDropdownButtonImageN);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_PROP_GRID_DROPDOWN_BUTTON_P);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &comboDropdownButtonImageP);

	Init();
	return 0;
}

void PropGridCtrl::Init()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (GetGlobalData()->m_hcurStretch == nullptr)
	{
		GetGlobalData()->m_hcurStretch = AfxGetApp()->LoadCursorW(AFX_IDC_HSPLITBAR);
	}

	if (GetGlobalData()->m_hcurStretchVert == nullptr)
	{
		GetGlobalData()->m_hcurStretchVert = AfxGetApp()->LoadCursorW(AFX_IDC_VSPLITBAR);
	}

	InitHeader();

	HDITEMW hdItem;
	hdItem.mask = HDI_TEXT | HDI_FORMAT;
	hdItem.fmt = HDF_LEFT;
	hdItem.pszText = _T("Property");
	hdItem.cchTextMax = 100;

	GetHeaderCtrl().InsertItem(0, &hdItem);

	hdItem.pszText = _T("Value");
	hdItem.cchTextMax = 100;

	GetHeaderCtrl().InsertItem(1, &hdItem);

	mVertScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_VERT, rectDummy, this, WND_ID_SCROLL_VERT);

	mToolTip.Create(this, TTS_ALWAYSTIP);
	mToolTip.Activate(TRUE);
	if (GetGlobalData()->m_nMaxToolTipWidth != -1)
	{
		mToolTip.SetMaxTipWidth(GetGlobalData()->m_nMaxToolTipWidth);
	}

	mToolTip.SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);

	mInplaceToolTip.Create(this);

	CWnd* pWndParent = GetParent();
	mUseControlBarColors = pWndParent == nullptr || !pWndParent->IsKindOf(RUNTIME_CLASS(CDialog));

	AdjustLayout();
	CreateBoldFont();
	CalcEditMargin();
}

void PropGridCtrl::InitHeader()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	mHeader.Create(WS_CHILD | WS_VISIBLE | HDS_HORZ, rectDummy, this, WND_ID_HEADER);
}

void PropGridCtrl::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CClientDC dc(this);
	HFONT hfontOld = SetCurrFont(&dc);

	TEXTMETRIC tm;
	dc.GetTextMetricsW(&tm);
	mRowHeight = tm.tmHeight + 6; // originally 4

	mHeaderHeight = 0;

	CRect rectClient;
	GetClientRect(rectClient);

	if (mEnableHeaderCtrl)
	{
		mHeaderHeight = mRowHeight + 4;

		GetHeaderCtrl().SendMessageW(
			WM_SETFONT, (WPARAM)(mFont != nullptr ? mFont : ::GetStockObject(DEFAULT_GUI_FONT)));
		GetHeaderCtrl().SetWindowPos(
			nullptr,
			rectClient.left,
			rectClient.top,
			rectClient.Width(),
			mHeaderHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);

		HDITEMW hdItem;
		hdItem.mask = HDI_WIDTH ;
		hdItem.cxy = mLeftColumnWidth + 2;

		GetHeaderCtrl().SetItem(0, &hdItem);

		hdItem.cxy = rectClient.Width() + 10;
		GetHeaderCtrl().SetItem(1, &hdItem);

		GetHeaderCtrl().ShowWindow(SW_SHOWNOACTIVATE);
	}
	else
	{
		GetHeaderCtrl().ShowWindow(SW_HIDE);
	}

	::SelectObject(dc.GetSafeHdc(), hfontOld);

	mRectList = rectClient;
	mRectList.top += mHeaderHeight;

	if (mEnableDescriptionArea && mDescrHeight != -1 && rectClient.Height() > 0)
	{
		mDescrHeight = std::max(mDescrHeight, mRowHeight);
		mDescrHeight = std::min(mDescrHeight, rectClient.Height() - mRowHeight);
		mRectList.bottom -= mDescrHeight;
	}

	int cxScroll = ::GetSystemMetrics(SM_CXHSCROLL);
	SetScrollSizes();

	if (mVertScrollTotal > 0)
	{
		// XS-NOTE: do not show scroll bar, mouse wheel still works
		//mRectList.right -= cxScroll;
		//mVertScrollBar.SetWindowPos(
		//	NULL,
		//	mRectList.right,
		//	mRectList.top,
		//	cxScroll,
		//	mRectList.Height(),
		//	SWP_NOZORDER | SWP_NOACTIVATE);
		mVertScrollBar.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		mVertScrollBar.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	ReposProperties();

	if (mSelection != nullptr && mSelection->HasButton())
	{
		ASSERT_VALID(mSelection);
		mSelection->AdjustButtonRect();
	}

	RedrawWindow();
}

void PropGridCtrl::ReposProperties()
{
	ASSERT_VALID(this);
	mTerminalProps.RemoveAll();

	if (mToolTip.GetSafeHwnd() != nullptr)
	{
		while (mTooltipsCount > 0)
		{
			mToolTip.DelTool(this, mTooltipsCount);
			mTooltipsCount--;
		}
	}

	int y = mRectList.top - mRowHeight * mVertScrollOffset - 1;

	if (!mAlphabeticMode)
	{
		for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; )
		{
			PropGridProperty* pProp = mProps.GetNext(pos);
			ASSERT_VALID(pProp);

			pProp->Reposition(y);
		}

		return;
	}

	POSITION pos = nullptr;

	// Get sorted list of terminal properties:
	for (pos = mProps.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = mProps.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->AddTerminalProp(mTerminalProps);
	}

	for (pos = mTerminalProps.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = mTerminalProps.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->Reposition(y);
	}
}

void PropGridCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	EndEditItem();

	mLeftColumnWidth = cx / 2;
	AdjustLayout();
}

void PropGridCtrl::OnSetFont(CFont* pFont, BOOL /*bRedraw*/)
{
	mFont = (HFONT)pFont->GetSafeHandle();

	CreateBoldFont();
	CalcEditMargin();
	AdjustLayout();
}

HFONT PropGridCtrl::OnGetFont()
{
	return (mFont != nullptr) ? mFont : (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

void PropGridCtrl::CreateBoldFont()
{
	if (mFontBold.GetSafeHandle() != nullptr)
	{
		mFontBold.DeleteObject();
	}

	CFont* pFont = CFont::FromHandle(
		mFont != nullptr ? mFont :(HFONT) ::GetStockObject(DEFAULT_GUI_FONT));
	ASSERT_VALID(pFont);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	pFont->GetLogFont(&lf);

	lf.lfWeight = FW_BOLD;
	mFontBold.CreateFontIndirectW(&lf);
}

void PropGridCtrl::CalcEditMargin()
{
	CEdit editDummy;
	editDummy.Create(WS_CHILD, CRect(0, 0, 100, 20), this, (UINT)-1);

	editDummy.SetFont(GetFont());
	mEditLeftMargin = LOWORD(editDummy.GetMargins());

	editDummy.SetFont(&mFontBold);
	mBoldEditLeftMargin = LOWORD(editDummy.GetMargins());

	editDummy.DestroyWindow();
}

HFONT PropGridCtrl::SetCurrFont(CDC* pDC)
{
	ASSERT_VALID(pDC);

	return (HFONT)SelectObject(
		pDC->GetSafeHdc(), mFont != nullptr ? mFont : GetStockObject(DEFAULT_GUI_FONT));
}

void PropGridCtrl::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
	OnDraw(&dcPaint);
}

void PropGridCtrl::OnDraw(CDC* pDCSrc)
{
	ASSERT_VALID(pDCSrc);

	CMemDC memDC(*pDCSrc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClient;
	GetClientRect(rectClient);

	OnFillBackground(pDC, rectClient);

	HFONT hfontOld = SetCurrFont(pDC);
	pDC->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_LEVEL1));
	pDC->SetBkMode(TRANSPARENT);

	OnDrawList(pDC);

	if (mEnableDescriptionArea)
	{
		CRect rectDescr = rectClient;
		rectDescr.top = mRectList.bottom;
		if (rectDescr.Height() > 0)
		{
			rectDescr.DeflateRect(1, 1);
			OnDrawDescription(pDC, rectDescr);
		}
	}

	SelectObject(pDC->GetSafeHdc(), hfontOld);
}

void PropGridCtrl::OnFillBackground(CDC* dc, CRect rectClient)
{
	dc->FillRect(&rectClient, theVisualManager->GetSemiWhiteBrush());
}

void PropGridCtrl::OnDrawBorder(CDC* /*pDC*/)
{
	ASSERT(FALSE); // This method is obsolete
}

void PropGridCtrl::OnDrawList(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CPen* originalPen = pDC->SelectObject(theVisualManager->GetLevel3Pen());

	int nXCenter = mRectList.left + mLeftColumnWidth;

	pDC->MoveTo(nXCenter, mRectList.top - 1);
	pDC->LineTo(nXCenter, mRectList.bottom);

	const CList<PropGridProperty*, PropGridProperty*>& lst = mAlphabeticMode ? mTerminalProps : mProps;

	for (POSITION pos = lst.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = lst.GetNext(pos);
		ASSERT_VALID(pProp);

		if (!OnDrawProperty(pDC, pProp))
		{
			break;
		}
	}

	pDC->SelectObject(originalPen);
}

void PropGridCtrl::OnDrawDescription(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	if (mDescriptionBackgroundColor != (COLORREF)-1)
	{
		CBrush br(mDescriptionBackgroundColor);
		pDC->FillRect(rect, &br);
	}
	else
	{
		pDC->FillRect(rect, mUseControlBarColors ? &(GetGlobalData()->brBarFace) : &(GetGlobalData()->brBtnFace));
	}

	rect.top += PGC_TEXT_MARGIN;

	COLORREF clrShadow = mUseControlBarColors ? GetGlobalData()->clrBarShadow : GetGlobalData()->clrBtnShadow;
	pDC->Draw3dRect(rect, clrShadow, clrShadow);

	if (mSelection == nullptr)
	{
		return;
	}

	rect.DeflateRect(PGC_TEXT_MARGIN, PGC_TEXT_MARGIN);

	ASSERT_VALID(mSelection);

	COLORREF clrTextOld = (COLORREF)-1;

	if (mDescriptionTextColor != (COLORREF)-1)
	{
		clrTextOld = pDC->SetTextColor(mDescriptionTextColor);
	}

	mSelection->OnDrawDescription(pDC, rect);

	if (clrTextOld == (COLORREF)-1)
	{
		pDC->SetTextColor(clrTextOld);
	}
}

BOOL PropGridCtrl::OnDrawProperty(CDC* pDC, PropGridProperty* pProp) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(pProp);

	if (!pProp->mRect.IsRectEmpty())
	{
		if (pProp->mRect.top >= mRectList.bottom)
		{
			return FALSE;
		}

		if (pProp->mRect.bottom >= mRectList.top)
		{
			const int nXCenter = mRectList.left + mLeftColumnWidth;
			COLORREF clrTextOld = (COLORREF)-1;

			if (mVSDotNetLook)
			{
				CRect rectLeft = pProp->mRect;

				if (!pProp->IsGroup())
				{
					rectLeft.right = std::min<int>(nXCenter, rectLeft.left);
				}

				if (pProp->mIsValueList)
				{
					rectLeft.right = rectLeft.left + rectLeft.Height();
				}

				rectLeft.left = mRectList.left;
				rectLeft.bottom = std::min(rectLeft.bottom, mRectList.bottom);

				if (rectLeft.left < rectLeft.right)
				{
					CBrush br(mGroupBackgroundColor != (COLORREF)-1 ? mGroupBackgroundColor : mGrayColor);
					pDC->FillRect(rectLeft, &br);
				}
			}

			if (!pProp->IsEnabled())
			{
				clrTextOld = pDC->SetTextColor(VisualManager::ToGdiColor(VisualManager::CLR_LEVEL2));
			}

			CRect rectName = pProp->mRect;

			if ((!pProp->IsGroup() || pProp->mIsValueList || !mGroupNameFullWidth) && pProp->HasValueField())
			{
				rectName.right = nXCenter;
			}

			if (pProp->IsGroup())
			{
				if (mGroupNameFullWidth && !mVSDotNetLook && !pProp->mIsValueList)
				{
					CRect rectFill = rectName;
					rectFill.top++;

					// XS-NOTE: group background
					pDC->FillRect(&rectFill, theVisualManager->GetBaseBrush());
				}

				CRect rectExpand = rectName;
				rectName.left += mRowHeight;
				rectExpand.right = rectName.left;

				CRgn rgnClipExpand;
				CRect rectExpandClip = rectExpand;
				rectExpandClip.bottom = std::min(rectExpandClip.bottom, mRectList.bottom);

				rgnClipExpand.CreateRectRgnIndirect(&rectExpandClip);
				pDC->SelectClipRgn(&rgnClipExpand);

				pProp->OnDrawExpandBox(pDC, rectExpand);
			}
			else if (!pProp->HasValueField())
			{
				CRect rectFill = rectName;
				rectFill.top++;

				if (mBackgroundBrush.GetSafeHandle() != nullptr)
				{
					CBrush& br = ((PropGridCtrl*)this)->mBackgroundBrush;
					pDC->FillRect(rectFill, &br);
				}
				else
				{
					pDC->FillRect(rectFill, &(GetGlobalData()->brWindow));
				}
			}

			if (rectName.right > rectName.left)
			{
				CRgn rgnClipName;
				CRect rectNameClip = rectName;
				rectNameClip.bottom = std::min(rectNameClip.bottom, mRectList.bottom);

				rgnClipName.CreateRectRgnIndirect(&rectNameClip);
				pDC->SelectClipRgn(&rgnClipName);

				HFONT hOldFont = nullptr;
				if (pProp->IsGroup() && !pProp->mIsValueList)
				{
					hOldFont = static_cast<HFONT>(::SelectObject(pDC->GetSafeHdc(), mFontBold.GetSafeHandle()));
				}

				pProp->OnDrawName(pDC, rectName);

				if (hOldFont != nullptr)
				{
					::SelectObject(pDC->GetSafeHdc(), hOldFont);
				}
			}

			CRect rectValue = pProp->mRect;
			rectValue.left = nXCenter + 1;

			CRgn rgnClipVal;
			CRect rectValClip = rectValue;
			rectValClip.bottom = std::min(rectValClip.bottom, mRectList.bottom);

			rgnClipVal.CreateRectRgnIndirect(&rectValClip);
			pDC->SelectClipRgn(&rgnClipVal);

			// XS-NOTE: edit background synchronization
			if (pProp->IsInPlaceEditing())
			{
				CRect editRect(rectValue);
				editRect.top += 1;
				pDC->FillRect(&editRect, theVisualManager->GetWhiteBrush());
			}
			pProp->OnDrawValue(pDC, rectValue);

			if (!pProp->mRectButton.IsRectEmpty())
			{
				pProp->OnDrawButton(pDC, pProp->mRectButton);
			}

			pDC->SelectClipRgn(nullptr);

			pDC->MoveTo(mRectList.left, pProp->mRect.bottom);
			pDC->LineTo(mRectList.right, pProp->mRect.bottom);

			if (clrTextOld != (COLORREF)-1)
			{
				pDC->SetTextColor(clrTextOld);
			}
		}
	}

	if (pProp->IsExpanded() || mAlphabeticMode)
	{
		for (POSITION pos = pProp->mSubItems.GetHeadPosition(); pos != nullptr; )
		{
			if (!OnDrawProperty(pDC, pProp->mSubItems.GetNext(pos)))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void PropGridCtrl::OnPropertyChanged(PropGridProperty* pProp)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pProp);

	pProp->SetModifiedFlag();

	DWORD_PTR propData = pProp->GetData();
	if (propData)
	{
		IMediator* mediator = reinterpret_cast<IMediator*>(propData);
		mediator->ToModel(pProp);
	}
}

BOOL PropGridCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void PropGridCtrl::OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADERW* pHeader = (NMHEADERW*) pNMHDR;
	ENSURE(pHeader != nullptr);

	if (pHeader->iItem == 0)
	{
		HDITEMW hdItem;
		hdItem.mask = HDI_WIDTH;

		GetHeaderCtrl().GetItem(0, &hdItem);

		CRect rectClient;
		GetClientRect(rectClient);

		mLeftColumnWidth = std::min(std::max(mRowHeight, hdItem.cxy - 2),
			rectClient.Width() - ::GetSystemMetrics(SM_CXHSCROLL) - 5);

		ReposProperties();

		InvalidateRect(mRectList);
		UpdateWindow();
	}

	*pResult = 0;
}

void PropGridCtrl::EnableHeaderCtrl(BOOL enable, LPCTSTR leftColumn, LPCTSTR rightColumn)
{
	ASSERT_VALID(this);
	ENSURE(leftColumn != nullptr);
	ENSURE(rightColumn != nullptr);

	PropGridProperty* pProp = GetCurSel();
	if (pProp != nullptr)
	{
		pProp->OnEndEdit();
	}

	mEnableHeaderCtrl = enable;

	if (mEnableHeaderCtrl)
	{
		HDITEMW hdItem;
		hdItem.mask = HDI_TEXT;

		hdItem.pszText = const_cast<LPTSTR>(leftColumn);
		hdItem.cchTextMax = static_cast<int>(_tcslen(leftColumn)) + 1;
		GetHeaderCtrl().SetItem(0, &hdItem);

		hdItem.pszText = const_cast<LPTSTR>(rightColumn);
		hdItem.cchTextMax = static_cast<int>(_tcslen(rightColumn)) + 1;
		GetHeaderCtrl().SetItem(1, &hdItem);
	}

	AdjustLayout();
	RedrawWindow();
}

void PropGridCtrl::EnableDescriptionArea(BOOL bEnable)
{
	ASSERT_VALID(this);

	mEnableDescriptionArea = bEnable;

	AdjustLayout();

	if (GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

void PropGridCtrl::OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADERW* pHeader = (NMHEADERW*)pNMHDR;
	ENSURE(pHeader != nullptr);

	pHeader->pitem->cxy = min(pHeader->pitem->cxy, mRectList.Width());

	TrackHeader(pHeader->pitem->cxy);
	*pResult = 0;
}

void PropGridCtrl::OnHeaderEndTrack(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	TrackHeader(-1);
	*pResult = 0;
}

void PropGridCtrl::OnSpinDeltaPos(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;

	PostMessage(PGC_WM_UPDATESPIN);
}

LRESULT PropGridCtrl::OnUpdateSpin(WPARAM, LPARAM)
{
	if (mSelection != nullptr && mSelection->mInPlaceEditing && mSelection->mEnabled)
	{
		mSelection->OnUpdateValue();
	}

	return 0;
}

void PropGridCtrl::TrackHeader(int nOffset)
{
	CClientDC dc(this);

	if (!mRectTrackHeader.IsRectEmpty() && !mShowDragContext)
	{
		dc.InvertRect(mRectTrackHeader);
	}

	if (nOffset < 0) // End of track
	{
		mRectTrackHeader.SetRectEmpty();
	}
	else
	{
		mRectTrackHeader = mRectList;
		mRectTrackHeader.left += nOffset;
		mRectTrackHeader.right = mRectTrackHeader.left + 1;

		if (mShowDragContext)
		{
			CRect rectClient;
			GetClientRect(rectClient);

			mLeftColumnWidth = std::min(
				std::max(mRowHeight, nOffset),
				rectClient.Width() - ::GetSystemMetrics(SM_CXHSCROLL) - 5);

			HDITEMW hdItem;
			hdItem.mask = HDI_WIDTH ;
			hdItem.cxy = mLeftColumnWidth + 2;

			GetHeaderCtrl().SetItem(0, &hdItem);

			hdItem.cxy = rectClient.Width() + 10;
			GetHeaderCtrl().SetItem(1, &hdItem);
		}
		else
		{
			dc.InvertRect(mRectTrackHeader);
		}
	}
}

void PropGridCtrl::TrackDescr(int nOffset)
{
	CClientDC dc(this);

	if (!mRectTrackDescr.IsRectEmpty())
	{
		dc.InvertRect(mRectTrackDescr);
	}

	if (nOffset == INT_MIN) // End of track
	{
		mRectTrackDescr.SetRectEmpty();
	}
	else
	{
		CRect rectClient;
		GetClientRect(rectClient);

		nOffset = std::max<int>(nOffset, rectClient.top + mRowHeight + mHeaderHeight);
		nOffset = std::min<int>(nOffset, rectClient.bottom - mRowHeight);

		mRectTrackDescr = rectClient;
		mRectTrackDescr.top = nOffset - 1;
		mRectTrackDescr.bottom = mRectTrackDescr.top + 2;

		dc.InvertRect(mRectTrackDescr);
	}
}

void PropGridCtrl::TrackToolTip(CPoint point)
{
	if (mTracking || mTrackingDescr)
	{
		return;
	}

	CPoint ptScreen = point;
	ClientToScreen(&ptScreen);

	CRect rectTT;
	mInplaceToolTip.GetWindowRect(&rectTT);

	if (rectTT.PtInRect(ptScreen) && mInplaceToolTip.IsWindowVisible())
	{
		return;
	}

	if (!mInplaceToolTip.IsWindowVisible())
	{
		rectTT.SetRectEmpty();
	}

	if (::GetCapture() == GetSafeHwnd())
	{
		ReleaseCapture();
	}

	PropGridProperty* pProp = HitTest(point);
	if (pProp == nullptr)
	{
		mInplaceToolTip.Deactivate();
		return;
	}

	if (abs(point.x -(mRectList.left + mLeftColumnWidth)) <= PGC_STRETCH_DELTA)
	{
		mInplaceToolTip.Deactivate();
		return;
	}

	ASSERT_VALID(pProp);

	if (pProp->IsInPlaceEditing())
	{
		return;
	}

	CString strTipText;
	CRect rectToolTip = pProp->mRect;

	BOOL bIsValueTT = FALSE;

	if (point.x < mRectList.left + mLeftColumnWidth)
	{
		if (pProp->IsGroup())
		{
			rectToolTip.left += mRowHeight;

			if (point.x <= rectToolTip.left)
			{
				mInplaceToolTip.Deactivate();
				return;
			}
		}

		if (pProp->mNameIsTruncated)
		{
			if (!mAlwaysShowUserTT || pProp->GetNameTooltip().IsEmpty())
			{
				strTipText = pProp->mName;
			}
		}
	}
	else
	{
		if (pProp->mValueIsTruncated)
		{
			if (!mAlwaysShowUserTT || pProp->GetValueTooltip().IsEmpty())
			{
				strTipText = pProp->FormatProperty();
			}
		}

		rectToolTip.left = mRectList.left + mLeftColumnWidth + 1;
		bIsValueTT = TRUE;
	}

	if (!strTipText.IsEmpty())
	{
		ClientToScreen(&rectToolTip);

		if (rectTT.TopLeft() == rectToolTip.TopLeft())
		{
			// Tooltip on the same place, don't show it to prevent flashing
			return;
		}

		mInplaceToolTip.SetTextMargin(PGC_TEXT_MARGIN);

		mInplaceToolTip.SetFont(
			bIsValueTT && pProp->IsModified() && mMarkModifiedProperties ?
			&mFontBold : GetFont());

		rectToolTip.left -= 1;
		rectToolTip.bottom += 1;
		mInplaceToolTip.Track(rectToolTip, strTipText);
		SetCapture();
	}
	else
	{
		mInplaceToolTip.Deactivate();
	}
}

int PropGridCtrl::AddProperty(PropGridProperty* pProp, BOOL bRedraw, BOOL bAdjustLayout)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pProp);

	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pListProp = mProps.GetNext(pos);
		ASSERT_VALID(pListProp);

		if (pListProp == pProp || pListProp->IsSubItem(pProp))
		{
			// Can't ad the same property twice
			ASSERT(FALSE);
			return -1;
		}
	}

	pProp->SetOwnerList(this);

	mProps.AddTail(pProp);
	int nIndex = (int) mProps.GetCount() - 1;

	if (bAdjustLayout)
	{
		AdjustLayout();

		if (bRedraw && GetSafeHwnd() != nullptr)
		{
			pProp->Redraw();
		}
	}

	return nIndex;
}

void PropGridCtrl::RemoveAll()
{
	ASSERT_VALID(this);

	while (!mProps.IsEmpty())
	{
		delete mProps.RemoveHead();
	}

	while (mTooltipsCount > 0)
	{
		mToolTip.DelTool(this, mTooltipsCount);
		mTooltipsCount--;
	}

	mTerminalProps.RemoveAll();

	mSelection = nullptr;
}

PropGridProperty* PropGridCtrl::GetProperty(int nIndex) const
{
	ASSERT_VALID(this);

	if (nIndex < 0 || nIndex >= mProps.GetCount())
	{
		ASSERT(FALSE);
		return nullptr;
	}

	return mProps.GetAt(mProps.FindIndex(nIndex));
}

PropGridProperty* PropGridCtrl::FindItemByData(
	DWORD_PTR dwData, BOOL bSearchSubItems/* = TRUE*/) const
{
	ASSERT_VALID(this);

	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = mProps.GetNext(pos);
		ASSERT_VALID(pProp);

		if (pProp->mData == dwData)
		{
			return pProp;
		}

		if (bSearchSubItems)
		{
			pProp = pProp->FindSubItemByData(dwData);

			if (pProp != nullptr)
			{
				ASSERT_VALID(pProp);
				return pProp;
			}
		}
	}

	return nullptr;
}

PropGridProperty* PropGridCtrl::HitTest(
	CPoint pt, PropGridProperty::ClickArea* pnArea, BOOL bPropsOnly) const
{
	ASSERT_VALID(this);

	if (!mRectList.PtInRect(pt) && !bPropsOnly)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		CRect rectDescr = rectClient;
		rectDescr.top = mRectList.bottom;

		if (pnArea != nullptr && rectDescr.PtInRect(pt))
		{
			*pnArea = PropGridProperty::ClickDescription;
		}

		return nullptr;
	}

	const CList<PropGridProperty*, PropGridProperty*>&
		lst = mAlphabeticMode ? mTerminalProps : mProps;

	for (POSITION pos = lst.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = lst.GetNext(pos);
		ASSERT_VALID(pProp);

		PropGridProperty* pHit = pProp->HitTest(pt, pnArea);
		if (pHit != nullptr)
		{
			return pHit;
		}
	}

	return nullptr;
}

void PropGridCtrl::SetCurSel(PropGridProperty* pProp, BOOL bRedraw)
{
	ASSERT_VALID(this);

	PropGridProperty* pOldSelectedItem = mSelection;
	if (pOldSelectedItem == pProp)
	{
		NotifyAccessibility(mSelection);
		return;
	}

	if (mSelection != nullptr && mSelection->mInPlaceEditing)
	{
		EndEditItem();
	}

	mSelection = pProp;
	OnChangeSelection(mSelection, pOldSelectedItem);

	if (pOldSelectedItem != nullptr)
	{
		pOldSelectedItem->OnKillSelection(pProp);

		CRect rectButton = pOldSelectedItem->mRectButton;
		pOldSelectedItem->mRectButton.SetRectEmpty();

		if (bRedraw)
		{
			CRect rectOld = pOldSelectedItem->mRect;

			if (!pOldSelectedItem->IsGroup() || !mGroupNameFullWidth)
			{
				if (!pOldSelectedItem->IsGroup() && pOldSelectedItem->HasValueField())
				{
					rectOld.right = rectOld.left + mLeftColumnWidth;
				}
			}

			rectOld.right = max(rectButton.right, rectOld.right);
			InvalidateRect(rectButton);
			InvalidateRect(rectOld);
		}
	}

	if (pProp != nullptr)
	{
		pProp->OnSetSelection(pOldSelectedItem);

		if (pProp->HasButton())
		{
			pProp->AdjustButtonRect();
		}

		if (bRedraw)
		{
			CRect rect = pProp->mRect;

			if (!pProp->IsGroup() || !mGroupNameFullWidth)
			{
				if (!pProp->IsGroup() && pProp->HasValueField())
				{
					rect.right = rect.left + mLeftColumnWidth;
				}
			}

			rect.right = max(pProp->mRectButton.right, rect.right);
			InvalidateRect(rect);
			InvalidateRect(pProp->mRectButton);
		}
	}

	if (bRedraw)
	{
		if (mEnableDescriptionArea)
		{
			CRect rectClient;
			GetClientRect(rectClient);

			CRect rectDescr = rectClient;
			rectDescr.top = mRectList.bottom;
			rectDescr.DeflateRect(PGC_TEXT_MARGIN, PGC_TEXT_MARGIN);
			InvalidateRect(rectDescr);
		}

		UpdateWindow();
	}

	NotifyAccessibility(mSelection);
}

void PropGridCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	SetFocus();

	CRect rectClient;
	GetClientRect(rectClient);

	if (mEnableDescriptionArea)
	{
		if (std::abs(point.y -(mRectList.bottom + PGC_TEXT_MARGIN)) <= PGC_STRETCH_DELTA)
		{
			SetCapture();
			TrackDescr(point.y);
			mTrackingDescr = TRUE;
			return;
		}

		if (point.y > mRectList.bottom)
		{
			return;
		}
	}

	if (std::abs(point.x -(mRectList.left + mLeftColumnWidth)) <= PGC_STRETCH_DELTA)
	{
		SetCapture();
		TrackHeader(point.x);
		mTracking = TRUE;
		return;
	}

	PropGridProperty::ClickArea clickArea;
	PropGridProperty* pHit = HitTest(point, &clickArea);

	BOOL bSelChanged = pHit != GetCurSel();

	SetCurSel(pHit);
	if (pHit == nullptr)
	{
		return;
	}

	if (mRectList.Height () > pHit->GetRect ().Height ())
	{
		EnsureVisible(pHit);
	}

	switch (clickArea)
	{
	case PropGridProperty::ClickExpandBox:
		pHit->Expand(!pHit->IsExpanded());
		break;

	case PropGridProperty::ClickName:
		pHit->OnClickName(point);
		break;

	case PropGridProperty::ClickValue:
		if (pHit->mEnabled)
		{
			if (EditItem(pHit, &point) && pHit->mWndInPlace != nullptr)
			{
				if (pHit->mRectButton.PtInRect(point))
				{
					CString strPrevVal = pHit->FormatProperty();

					if (::GetCapture() == GetSafeHwnd())
					{
						ReleaseCapture();
					}

					pHit->OnClickButton(point);

					if (strPrevVal != pHit->FormatProperty())
					{
						OnPropertyChanged(pHit);
					}
				}
				else if (!bSelChanged || pHit->IsProcessFirstClick())
				{
					pHit->OnClickValue(WM_LBUTTONDOWN, point);
				}
			}
		}
		break;

	default:
		break;
	}
}

void PropGridCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);

	SetFocus();

	CRect rectClient;
	GetClientRect(rectClient);

	if (mEnableDescriptionArea)
	{
		if (abs(point.y -(mRectList.bottom + PGC_TEXT_MARGIN)) <= PGC_STRETCH_DELTA)
		{
			return;
		}

		if (point.y > mRectList.bottom)
		{
			return;
		}
	}

	if (abs(point.x -(mRectList.left + mLeftColumnWidth)) <= PGC_STRETCH_DELTA)
	{
		return;
	}

	PropGridProperty::ClickArea clickArea;
	PropGridProperty* pHit = HitTest(point, &clickArea);

	BOOL bSelChanged = pHit != GetCurSel();

	SetCurSel(pHit);
	if (pHit == nullptr)
	{
		return;
	}

	EnsureVisible(pHit);

	switch (clickArea)
	{
	case PropGridProperty::ClickExpandBox:
		break;

	case PropGridProperty::ClickName:
		pHit->OnRClickName(point);
		break;

	case PropGridProperty::ClickValue:
		pHit->OnRClickValue(point, bSelChanged);

		if (pHit->mEnabled && !bSelChanged)
		{
			if (EditItem(pHit, &point) && pHit->mWndInPlace != nullptr)
			{
				if (pHit->mRectButton.PtInRect(point))
				{
					return;
				}
				else if (pHit->IsProcessFirstClick())
				{
					pHit->OnClickValue(WM_RBUTTONDOWN, point);
				}
			}
		}
		break;

	default:
		break;
	}
}

BOOL PropGridCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(GetGlobalData()->RegisterWindowClass(
		_T("Afx:PropList")), _T(""), dwStyle, rect, pParentWnd, nID, nullptr);
}

BOOL PropGridCtrl::EditItem(PropGridProperty* pProp, LPPOINT lptClick)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pProp);

	if (!EndEditItem())
	{
		return FALSE;
	}

	if (pProp->IsGroup() && !pProp->mIsValueList)
	{
		return FALSE;
	}

	if (pProp->OnEdit(lptClick))
	{
		pProp->Redraw();
		SetCurSel(pProp);
		SetCapture();
	}

	return TRUE;
}

void PropGridCtrl::OnClickButton(CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mSelection);

	if (mSelection->OnUpdateValue())
	{
		CString strPrevVal = mSelection->FormatProperty();

		CWaitCursor wait;
		mSelection->OnClickButton(point);

		if (strPrevVal != mSelection->FormatProperty())
		{
			OnPropertyChanged(mSelection);
		}
	}
}

BOOL PropGridCtrl::EndEditItem(BOOL bUpdateData/* = TRUE*/)
{
	ASSERT_VALID(this);

	if (mSelection == nullptr)
	{
		return TRUE;
	}

	ASSERT_VALID(mSelection);

	if (!mSelection->mInPlaceEditing)
	{
		return TRUE;
	}

	if (bUpdateData)
	{
		if (!ValidateItemData(mSelection) || !mSelection->OnUpdateValue())
		{
			return FALSE;
		}
	}

	if (mSelection != nullptr && !mSelection->OnEndEdit())
	{
		return FALSE;
	}

	if (::GetCapture() == GetSafeHwnd())
	{
		ReleaseCapture();
	}

	if (mSelection != nullptr)
	{
		mSelection->Redraw();
	}

	return TRUE;
}

BOOL PropGridCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_NCLBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
		mToolTip.RelayEvent(pMsg);
		mInplaceToolTip.Hide();
		break;

	case WM_MOUSEMOVE:
		mToolTip.RelayEvent(pMsg);

		if (pMsg->wParam == 0) // No buttons pressed
		{
			CPoint ptCursor;
			::GetCursorPos(&ptCursor);
			ScreenToClient(&ptCursor);

			TrackToolTip(ptCursor);
		}
		break;
	}

	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_TAB &&
		mSelection != nullptr &&
		mSelection->OnActivateByTab())
	{
		return TRUE;
	}

	if (pMsg->message == WM_SYSKEYDOWN &&
		(pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RIGHT) &&
		mSelection != nullptr &&
		mSelection->mEnabled &&
		((mSelection->mFlags) & PropGridProperty::PROP_HAS_BUTTON) &&
		EditItem(mSelection))
	{
		CString strPrevVal = mSelection->FormatProperty();

		CWaitCursor wait;
		mSelection->OnClickButton(CPoint(-1, -1));

		if (strPrevVal != mSelection->FormatProperty())
		{
			OnPropertyChanged(mSelection);
		}

		return TRUE;
	}

	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN &&
		mSelection != nullptr &&
		mSelection->IsGroup() &&
		!mSelection->IsInPlaceEditing())
	{
		mSelection->Expand(!mSelection->IsExpanded());
		return TRUE;
	}

	if (mSelection != nullptr && mSelection->mInPlaceEditing && mSelection->mEnabled)
	{
		ASSERT_VALID(mSelection);

		const BOOL bIsDroppedDown =
			mSelection->mWndCombo != nullptr && mSelection->mWndCombo->GetDroppedState();

		if (pMsg->message == WM_KEYDOWN && (!bIsDroppedDown || pMsg->wParam == VK_RETURN))
		{
			switch (pMsg->wParam)
			{
			case VK_RETURN:
				if (mSelection->mButtonIsFocused)
				{
					CString strPrevVal = mSelection->FormatProperty();

					CWaitCursor wait;
					mSelection->OnClickButton(CPoint(-1, -1));

					if (strPrevVal != mSelection->FormatProperty())
					{
						OnPropertyChanged(mSelection);
					}
					return TRUE;
				}

				if (bIsDroppedDown)
				{
					HWND hwndInplace = mSelection->mWndInPlace->GetSafeHwnd();
					mSelection->OnSelectCombo();

					if (::IsWindow(hwndInplace))
					{
						mSelection->mWndInPlace->SetFocus();
					}
				}

				if (!EndEditItem())
				{
					MessageBeep((UINT)-1);
				}

				SetFocus();
				break;

			case VK_ESCAPE:
				EndEditItem(FALSE);
				SetFocus();
				break;

			case VK_DOWN:
			case VK_UP:
				if (mSelection->mOptions.GetCount() > 1)
				{
					if (mSelection->OnRotateListValue(pMsg->wParam != VK_UP))
					{
						return TRUE;
					}
				}
				else if (::IsWindow(mSelection->mWndInPlace->GetSafeHwnd()))
				{
					mSelection->mWndInPlace->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
				break;

			default:
				if (!mSelection->mAllowEdit)
				{
					mSelection->PushChar((UINT) pMsg->wParam);
					return TRUE;
				}

				if (ProcessClipboardAccelerators((UINT) pMsg->wParam))
				{
					return TRUE;
				}

				return FALSE;
			}

			return TRUE;
		}
		else if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
		{
			CPoint ptCursor;
			GetCursorPos(&ptCursor);
			ScreenToClient(&ptCursor);

			if (mSelection->mWndSpin != nullptr)
			{
				ASSERT_VALID(mSelection->mWndSpin);
				ASSERT(mSelection->mWndSpin->GetSafeHwnd() != nullptr);

				CRect rectSpin;
				mSelection->mWndSpin->GetClientRect(rectSpin);
				mSelection->mWndSpin->MapWindowPoints(this, rectSpin);

				if (rectSpin.PtInRect(ptCursor))
				{
					MapWindowPoints(mSelection->mWndSpin, &ptCursor, 1);

					mSelection->mWndSpin->SendMessageW(
						pMsg->message, pMsg->wParam, MAKELPARAM(ptCursor.x, ptCursor.y));
					return TRUE;
				}
			}

			CWnd* pWndInPlaceEdit = mSelection->mWndInPlace;
			ASSERT_VALID(pWndInPlaceEdit);

			if (!mSelection->mAllowEdit)
			{
				mSelection->mWndInPlace->HideCaret();
			}

			CRect rectEdit;
			pWndInPlaceEdit->GetClientRect(rectEdit);
			pWndInPlaceEdit->MapWindowPoints(this, rectEdit);

			if (rectEdit.PtInRect(ptCursor) && pMsg->message == WM_LBUTTONDBLCLK)
			{
				if (mSelection->OnDblClk(ptCursor))
				{
					return TRUE;
				}
			}

			if (rectEdit.PtInRect(ptCursor) &&
				pMsg->message == WM_RBUTTONDOWN &&
				!mSelection->mAllowEdit)
			{
				return TRUE;
			}

			if (!rectEdit.PtInRect(ptCursor) &&
				(pMsg->message == WM_LBUTTONDOWN ||
				 pMsg->message == WM_NCLBUTTONDOWN ||
				 pMsg->message == WM_RBUTTONDOWN ||
				 pMsg->message == WM_MBUTTONDOWN))
			{
				if (mSelection->mRectButton.PtInRect(ptCursor))
				{
					CString strPrevVal = mSelection->FormatProperty();

					CWaitCursor wait;
					OnClickButton(ptCursor);

					if (strPrevVal != mSelection->FormatProperty())
					{
						OnPropertyChanged(mSelection);
					}
					return TRUE;
				}

				if (!EndEditItem())
				{
					return TRUE;
				}
			}
			else
			{
				MapWindowPoints(pWndInPlaceEdit, &ptCursor, 1);
				pWndInPlaceEdit->SendMessageW(
					pMsg->message, pMsg->wParam, MAKELPARAM(ptCursor.x, ptCursor.y));
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void PropGridCtrl::OnCancelMode()
{
	if (mTracking)
	{
		TrackHeader(-1);
		mTracking = FALSE;

		if (::GetCapture() == GetSafeHwnd())
		{
			ReleaseCapture();
		}
	}

	if (mTrackingDescr)
	{
		TrackDescr(INT_MIN);
		mTrackingDescr = FALSE;

		if (::GetCapture() == GetSafeHwnd())
		{
			ReleaseCapture();
		}
	}

	// Tooltip:
	if (::GetCapture() == GetSafeHwnd())
	{
		ReleaseCapture();
	}

	mInplaceToolTip.Deactivate();

	EndEditItem();

	CWnd::OnCancelMode();
}

void PropGridCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	mFocused = TRUE;

	if (mSelection != nullptr)
	{
		RedrawWindow(mSelection->mRect);
	}
}

void PropGridCtrl::OnKillFocus(CWnd* pNewWnd)
{
	if (!IsChild(pNewWnd))
	{
		if (mSelection == nullptr || mSelection->OnKillFocus(pNewWnd))
		{
			EndEditItem();
			mFocused = FALSE;

			if (mSelection != nullptr)
			{
				mSelection->Redraw();
			}
		}
	}

	CWnd::OnKillFocus(pNewWnd);
}

void PropGridCtrl::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	CWnd::OnStyleChanged(nStyleType, lpStyleStruct);
	AdjustLayout();
}

UINT PropGridCtrl::OnGetDlgCode()
{
	return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void PropGridCtrl::SetScrollSizes()
{
	ASSERT_VALID(this);

	if (mVertScrollBar.GetSafeHwnd() == nullptr)
	{
		return;
	}

	if (mRowHeight == 0)
	{
		mVertScrollPage = 0;
		mVertScrollTotal = 0;
		mVertScrollOffset = 0;
	}
	else
	{
		mVertScrollPage = mRectList.Height() / mRowHeight - 1;
		mVertScrollTotal = GetTotalItems(FALSE /* Visible only */);

		if (mVertScrollTotal <= mVertScrollPage)
		{
			mVertScrollPage = 0;
			mVertScrollTotal = 0;
		}

		mVertScrollOffset = min(mVertScrollOffset, mVertScrollTotal);
	}

	SCROLLINFO si;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);

	si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	si.nMin = 0;
	si.nMax = mVertScrollTotal;
	si.nPage = mVertScrollPage;
	si.nPos = mVertScrollOffset;

	SetScrollInfo(SB_VERT, &si, TRUE);
	mVertScrollBar.EnableScrollBar(mVertScrollTotal > 0 ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
	mVertScrollBar.EnableWindow();
}

int PropGridCtrl::GetTotalItems(BOOL bIncludeHidden) const
{
	ASSERT_VALID(this);

	int nCount = 0;

	if (mAlphabeticMode)
	{
		if (bIncludeHidden)
		{
			return(int) mTerminalProps.GetCount();
		}

		for (POSITION pos = mTerminalProps.GetHeadPosition(); pos != nullptr; )
		{
			PropGridProperty* pProp = mTerminalProps.GetNext(pos);
			ASSERT_VALID(pProp);

			if (pProp->IsVisible())
			{
				nCount++;

				if (pProp->IsExpanded())
				{
					nCount += pProp->GetExpandedSubItems();
				}
			}
		}

		return nCount;
	}

	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; )
	{
		PropGridProperty* pProp = mProps.GetNext(pos);
		ASSERT_VALID(pProp);

		nCount += pProp->GetExpandedSubItems(bIncludeHidden) + 1;
	}

	return nCount;
}

void PropGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (mSelection != nullptr &&
		pScrollBar->GetSafeHwnd() != nullptr &&
		mSelection->mWndSpin->GetSafeHwnd() == pScrollBar->GetSafeHwnd())
	{
		return;
	}

	mInplaceToolTip.Hide();
	EndEditItem();

	int nPrevOffset = mVertScrollOffset;

	switch (nSBCode)
	{
	case SB_LINEUP:
		mVertScrollOffset--;
		break;

	case SB_LINEDOWN:
		mVertScrollOffset++;
		break;

	case SB_TOP:
		mVertScrollOffset = 0;
		break;

	case SB_BOTTOM:
		mVertScrollOffset = mVertScrollTotal;
		break;

	case SB_PAGEUP:
		mVertScrollOffset -= mVertScrollPage;
		break;

	case SB_PAGEDOWN:
		mVertScrollOffset += mVertScrollPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		mVertScrollOffset = nPos;
		break;

	default:
		return;
	}

	mVertScrollOffset = std::min(std::max(0, mVertScrollOffset), mVertScrollTotal - mVertScrollPage + 1);
	if (mVertScrollOffset == nPrevOffset)
	{
		return; /* no change */
	}

	SetScrollPos(SB_VERT, mVertScrollOffset);

	ReposProperties();

	int dy = mRowHeight *(nPrevOffset - mVertScrollOffset);
	ScrollWindow(0, dy, mRectList, mRectList);

	if (mSelection != nullptr)
	{
		ASSERT_VALID(mSelection);
		RedrawWindow(mSelection->mRectButton);
	}
}

CScrollBar* PropGridCtrl::GetScrollBarCtrl(int nBar) const
{
	if (nBar == SB_HORZ || mVertScrollBar.GetSafeHwnd() == nullptr)
	{
		return nullptr;
	}

	return const_cast<CScrollBar*>(&mVertScrollBar);
}

BOOL PropGridCtrl::OnMouseWheel(UINT /*nFlags*/, short zDelta, CPoint /*pt*/)
{
	if (CMFCPopupMenu::GetActiveMenu() != nullptr)
	{
		return TRUE;
	}

	if (mVertScrollTotal > 0)
	{
		const int nSteps = abs(zDelta) / WHEEL_DELTA;

		for (int i = 0; i < nSteps; i++)
		{
			OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, nullptr);
		}
	}

	return TRUE;
}

void PropGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);

	if (point.y <= mRectList.bottom &&
		std::abs(point.x - (mRectList.left + mLeftColumnWidth)) <= PGC_STRETCH_DELTA)
	{
		// Double click on vertical splitter, make splitter 50/50:
		CRect rectClient;
		GetClientRect(rectClient);

		mLeftColumnWidth = rectClient.Width() / 2;

		HDITEMW hdItem;
		hdItem.mask = HDI_WIDTH ;
		hdItem.cxy = mLeftColumnWidth + 2;

		GetHeaderCtrl().SetItem(0, &hdItem);

		hdItem.cxy = rectClient.Width() + 10;
		GetHeaderCtrl().SetItem(1, &hdItem);
		return;
	}

	if (mSelection == nullptr)
	{
		return;
	}

	ASSERT_VALID(mSelection);

	if (mSelection->IsGroup() &&
		(!mSelection->mIsValueList || point.x < mRectList.left + mLeftColumnWidth))
	{
		mSelection->Expand(!mSelection->IsExpanded());
	}
	else if (mSelection->mEnabled)
	{
		if (EditItem(mSelection) && mSelection->mWndInPlace != nullptr)
		{
			mSelection->mWndInPlace->SendMessageW(WM_LBUTTONDOWN);
			mSelection->mWndInPlace->SendMessageW(WM_LBUTTONUP);
		}

		if (mSelection->GetRect().PtInRect(point))
		{
			mSelection->OnDblClk(point);
		}
	}
}

BOOL PropGridCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		GetCursorPos(&point);
		ScreenToClient(&point);

		if (mEnableDescriptionArea &&
			abs(point.y - (mRectList.bottom + PGC_TEXT_MARGIN)) <= PGC_STRETCH_DELTA)
		{
			SetCursor(GetGlobalData()->m_hcurStretchVert);
			return TRUE;
		}

		if (point.y <= mRectList.bottom)
		{
			if (abs(point.x - (mRectList.left + mLeftColumnWidth)) <= PGC_STRETCH_DELTA)
			{
				SetCursor(GetGlobalData()->m_hcurStretch);
				return TRUE;
			}

			PropGridProperty::ClickArea clickArea;
			PropGridProperty* pHit = HitTest(point, &clickArea);

			if (pHit != nullptr &&
				pHit == mSelection &&
				clickArea == PropGridProperty::ClickValue &&
				!pHit->mRectButton.PtInRect(point) &&
				pHit->OnSetCursor())
			{
				return TRUE;
			}
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void PropGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (mProps.IsEmpty())
	{
		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	switch (nChar)
	{
	case VK_F4:
		if (mSelection != nullptr && mSelection->mEnabled && EditItem(mSelection))
		{
			if ((mSelection->mFlags) & PropGridProperty::PROP_HAS_BUTTON)
			{
				CString strPrevVal = mSelection->FormatProperty();

				CWaitCursor wait;
				mSelection->OnClickButton(CPoint(-1, -1));

				if (strPrevVal != mSelection->FormatProperty())
				{
					OnPropertyChanged(mSelection);
				}
			}
			return;
		}
		break;

	case VK_LEFT:
		if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			BOOL bShowDragContext = mShowDragContext;
			mShowDragContext = TRUE;

			TrackHeader(mLeftColumnWidth - 5);

			mShowDragContext = bShowDragContext;
			return;
		}
		else if (mSelection != nullptr && mSelection->IsGroup() && mSelection->IsExpanded())
		{
			mSelection->Expand(FALSE);
			return;
		}

		// else ==> act as VK_UP!

	case VK_UP:
		{
			if (mSelection == nullptr)
			{
				SetCurSel(mAlphabeticMode ? mTerminalProps.GetHead() : mProps.GetHead());
				OnVScroll(SB_TOP, 0, nullptr);
				return;
			}

			// Select prev. item:
			CPoint point(mSelection->mRect.right - 1, mSelection->mRect.top - 2);

			PropGridProperty* pHit = HitTest(point, nullptr, TRUE);
			if (pHit != nullptr)
			{
				SetCurSel(pHit);
				EnsureVisible(pHit);
			}
		}
		return;

	case VK_RIGHT:
		if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			BOOL bShowDragContext = mShowDragContext;
			mShowDragContext = TRUE;

			TrackHeader(mLeftColumnWidth + 5);

			mShowDragContext = bShowDragContext;
			return;
		}
		else if (mSelection != nullptr && mSelection->IsGroup() && !mSelection->IsExpanded())
		{
			mSelection->Expand();
			return;
		}

		// else ==> act as VK_DOWN!

	case VK_DOWN:
		{
			if (mSelection == nullptr)
			{
				SetCurSel(mAlphabeticMode ? mTerminalProps.GetHead() : mProps.GetHead());
				OnVScroll(SB_TOP, 0, nullptr);
				return;
			}

			if ((::GetAsyncKeyState(VK_MENU) & 0x8000) && nChar == VK_DOWN)
			{
				CString strPrevVal = mSelection->FormatProperty();

				CWaitCursor wait;
				mSelection->OnClickButton(CPoint(-1, -1));

				if (strPrevVal != mSelection->FormatProperty())
				{
					OnPropertyChanged(mSelection);
				}

				return;
			}

			// Select next item:
			CPoint point(mSelection->mRect.right - 1, mSelection->mRect.bottom + 2);

			PropGridProperty* pHit = HitTest(point, nullptr, TRUE);
			if (pHit != nullptr)
			{
				SetCurSel(pHit);
				EnsureVisible(pHit);
			}
		}
		return;

	case VK_NEXT:
		{
			if (mSelection == nullptr)
			{
				SetCurSel(mAlphabeticMode ? mTerminalProps.GetHead() : mProps.GetHead());
				OnVScroll(SB_TOP, 0, nullptr);
				return;
			}

			if (mVertScrollPage != 0)
			{
				EnsureVisible(mSelection);

				CPoint point(mSelection->mRect.right - 1, mSelection->mRect.top + mVertScrollPage * mRowHeight);

				PropGridProperty* pHit = HitTest(point, nullptr, TRUE);
				if (pHit != nullptr)
				{
					SetCurSel(pHit);
					OnVScroll(SB_PAGEDOWN, 0, nullptr);
					return;
				}
			}
		}

	case VK_END:
		{
			PropGridProperty* pLastProp = nullptr;

			if (mAlphabeticMode)
			{
				pLastProp = mTerminalProps.GetTail();
				ASSERT_VALID(pLastProp);
			}
			else
			{
				pLastProp = mProps.GetTail();
				ASSERT_VALID(pLastProp);
			}

			while (!pLastProp->mSubItems.IsEmpty() && pLastProp->IsExpanded())
			{
				pLastProp = pLastProp->mSubItems.GetTail();
			}

			SetCurSel(pLastProp);
			OnVScroll(SB_BOTTOM, 0, nullptr);
		}
		return;

	case VK_PRIOR:
		{
			if (mSelection != nullptr && mVertScrollPage != 0)
			{
				EnsureVisible(mSelection);

				CPoint point(mSelection->mRect.right - 1, mSelection->mRect.top - mVertScrollPage * mRowHeight);

				PropGridProperty* pHit = HitTest(point, nullptr, TRUE);
				if (pHit != nullptr)
				{
					SetCurSel(pHit);
					OnVScroll(SB_PAGEUP, 0, nullptr);
					return;
				}
			}
		}

	case VK_HOME:
		SetCurSel(mAlphabeticMode ? mTerminalProps.GetHead() : mProps.GetHead());
		OnVScroll(SB_TOP, 0, nullptr);
		return;

	case VK_ADD:
		if (mSelection != nullptr && mSelection->IsGroup() &&
			!mSelection->IsExpanded() && !mSelection->IsInPlaceEditing())
		{
			mSelection->Expand();
		}
		return;

	case VK_SUBTRACT:
		if (mSelection != nullptr && mSelection->IsGroup() &&
			mSelection->IsExpanded() && !mSelection->IsInPlaceEditing())
		{
			mSelection->Expand(FALSE);
		}
		return;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void PropGridCtrl::EnsureVisible(PropGridProperty* pProp, BOOL bExpandParents/* = FALSE*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pProp);

	if (mRowHeight == 0)
	{
		return;
	}

	if (bExpandParents && pProp->GetParent() != nullptr)
	{
		PropGridProperty* pParent = pProp;

		while ((pParent = pParent->GetParent()) != nullptr)
		{
			ASSERT_VALID(pParent);
			pParent->Expand(TRUE);
		}
	}

	CRect rect = pProp->mRect;

	if (rect.top >= mRectList.top - 1 && rect.bottom <= mRectList.bottom)
	{
		return;
	}

	CRect rectButton = pProp->mRectButton;
	pProp->mRectButton.SetRectEmpty();
	RedrawWindow(rectButton);

	if (rect.top < mRectList.top - 1 && rect.bottom >= mRectList.top - 1)
	{
		OnVScroll(SB_LINEUP, 0, nullptr);
	}
	else if (rect.bottom > mRectList.bottom && rect.top <= mRectList.bottom)
	{
		OnVScroll(SB_LINEDOWN, 0, nullptr);
	}
	else
	{
		OnVScroll(SB_THUMBPOSITION, rect.top / mRowHeight - mVertScrollOffset, nullptr);
	}

	if (!rectButton.IsRectEmpty())
	{
		pProp->AdjustButtonRect();
		RedrawWindow(pProp->mRectButton);
	}
}

void PropGridCtrl::ExpandAll(BOOL bExpand/* = TRUE*/)
{
	ASSERT_VALID(this);

	if (mAlphabeticMode)
	{
		return;
	}

	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* pProp = mProps.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->ExpandDeep(bExpand);
	}

	AdjustLayout();

	if (GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

void PropGridCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnChar(nChar, nRepCnt, nFlags);

	if (mSelection == nullptr || !mSelection->mEnabled)
	{
		return;
	}

	ASSERT_VALID(mSelection);

	if (!EditItem(mSelection))
	{
		return;
	}

	mSelection->PushChar(nChar);
}

HBRUSH PropGridCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	if (mSelection != nullptr && pWnd->GetSafeHwnd() == mSelection->mWndInPlace->GetSafeHwnd())
	{
		HBRUSH hbrProp = mSelection->OnCtlColor(pDC, nCtlColor);
		if (hbrProp != nullptr)
		{
			return hbrProp;
		}
	}

	return hbr;
}

void PropGridCtrl::UpdateColor(COLORREF color)
{
	ASSERT_VALID(this);
	ASSERT_VALID(mSelection);

	PropGridColorProperty* pColorProp = DYNAMIC_DOWNCAST(PropGridColorProperty, mSelection);
	if (pColorProp == nullptr)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bChanged = color != pColorProp->GetColor();
	pColorProp->SetColor(color);

	if (bChanged)
	{
		OnPropertyChanged(pColorProp);
	}

	if (color == (COLORREF)-1 &&
		pColorProp->mWndInPlace != nullptr &&
		::IsWindow(pColorProp->mWndInPlace->GetSafeHwnd()))
	{
		pColorProp->mWndInPlace->SetWindowText(_T(""));
	}

	pColorProp->OnUpdateValue();
}

void PropGridCtrl::CloseColorPopup()
{
	ASSERT_VALID(this);
	ASSERT_VALID(mSelection);

	PropGridColorProperty* pColorProp = DYNAMIC_DOWNCAST(PropGridColorProperty, mSelection);
	if (pColorProp == nullptr)
	{
		ASSERT(FALSE);
		return;
	}

	pColorProp->mPopup = nullptr;

	pColorProp->mButtonIsDown = FALSE;
	pColorProp->Redraw();

	if (pColorProp->mWndInPlace != nullptr)
	{
		pColorProp->mWndInPlace->SetFocus();
	}
}

void PropGridCtrl::OnSelectCombo()
{
	ASSERT_VALID(this);

	if (mSelection == nullptr)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT_VALID(mSelection);
	mSelection->OnSelectCombo();
}

void PropGridCtrl::OnCloseCombo()
{
	ASSERT_VALID(this);

	if (mSelection == nullptr)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT_VALID(mSelection);
	mSelection->OnCloseCombo();
}

void PropGridCtrl::OnEditKillFocus()
{
	if (mSelection != nullptr && mSelection->mInPlaceEditing && mSelection->mEnabled)
	{
		ASSERT_VALID(mSelection);

		if (!IsChild(GetFocus()) && mSelection->OnEditKillFocus())
		{
			if (!EndEditItem())
			{
				mSelection->mWndInPlace->SetFocus();
			}
			else
			{
				OnKillFocus(GetFocus());
			}
		}
	}
}

void PropGridCtrl::OnComboKillFocus()
{
	if (mSelection != nullptr && mSelection->mWndCombo != nullptr && mSelection->mEnabled)
	{
		ASSERT_VALID(mSelection);

		if (!IsChild(GetFocus()))
		{
			if (!EndEditItem())
			{
				mSelection->mWndCombo->SetFocus();
			}
			else
			{
				OnKillFocus(GetFocus());
			}
		}
	}
}

void PropGridCtrl::SetBoolLabels(LPCTSTR lpszTrue, LPCTSTR lpszFalse)
{
	ASSERT_VALID(this);
	ASSERT(lpszTrue != nullptr);
	ASSERT(lpszFalse != nullptr);

	mTrueString = lpszTrue;
	mFalseString = lpszFalse;

	if (GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

void PropGridCtrl::SetListDelimiter(TCHAR c)
{
	ASSERT_VALID(this);
	ASSERT(GetSafeHwnd() == nullptr); // Should be called before window create

	mListDelimeter = c;
}

void PropGridCtrl::SetDescriptionRows(int nDescRows)
{
	ASSERT_VALID(this);

	mDescrRows = nDescRows;

	if (GetSafeHwnd() != nullptr)
	{
		AdjustLayout();
		RedrawWindow();
	}
}

void PropGridCtrl::SetAlphabeticMode(BOOL bSet)
{
	ASSERT_VALID(this);

	if (mAlphabeticMode == bSet)
	{
		return;
	}

	mAlphabeticMode = bSet;
	mVertScrollOffset = 0;

	SetCurSel(nullptr);

	if (GetSafeHwnd() != nullptr)
	{
		if (mAlphabeticMode)
		{
			ReposProperties();
		}

		AdjustLayout();
		RedrawWindow();
	}
}

void PropGridCtrl::SetVSDotNetLook(BOOL bSet)
{
	ASSERT_VALID(this);
	mVSDotNetLook = bSet;

	if (GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

BOOL PropGridCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CWnd::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ENSURE(pNMHDR != nullptr);

	if (pNMHDR->code == TTN_SHOW)
	{
		mToolTip.SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void PropGridCtrl::OnDestroy()
{
	if (comboDropdownButtonImageN)
	{
		comboDropdownButtonImageN->Release();
		comboDropdownButtonImageN = nullptr;
	}
	if (comboDropdownButtonImageP)
	{
		comboDropdownButtonImageP->Release();
		comboDropdownButtonImageP = nullptr;
	}

	while (!mProps.IsEmpty())
	{
		delete mProps.RemoveHead();
	}

	mSelection = nullptr;

	mInplaceToolTip.DestroyWindow();
	mToolTip.DestroyWindow();

	CWnd::OnDestroy();
}

BOOL PropGridCtrl::OnNeedTipText(UINT /*id*/, NMHDR* nmhr, LRESULT* /*pResult*/)
{
	static CString strTipText;

	if (mToolTip.GetSafeHwnd() == nullptr || nmhr->hwndFrom != mToolTip.GetSafeHwnd())
	{
		return FALSE;
	}

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);

	PropGridProperty* pProp = HitTest(point);
	if (pProp == nullptr)
	{
		return FALSE;
	}

	if (point.x < mRectList.left + mLeftColumnWidth)
	{
		if (!pProp->mNameIsTruncated || mAlwaysShowUserTT)
		{
			// User-defined tooltip
			strTipText = pProp->GetNameTooltip();
		}
	}
	else
	{
		if (!pProp->mValueIsTruncated || mAlwaysShowUserTT)
		{
			// User-defined tooltip
			strTipText = pProp->GetValueTooltip();
		}
	}

	if (strTipText.IsEmpty())
	{
		return FALSE;
	}

	LPNMTTDISPINFO pTTDispInfo = reinterpret_cast<LPNMTTDISPINFO>(nmhr);
	ASSERT((pTTDispInfo->uFlags & TTF_IDISHWND) == 0);

	pTTDispInfo->lpszText = const_cast<LPTSTR>((LPCTSTR) strTipText);

	mToolTip.SetFont(GetFont(), FALSE);
	return TRUE;
}

void PropGridCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if (mTracking)
	{
		TrackHeader(point.x);
		return;
	}

	if (mTrackingDescr)
	{
		TrackDescr(point.y);
		return;
	}
}

void PropGridCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);

	if (mTracking)
	{
		TrackHeader(-1);
		mTracking = FALSE;

		if (::GetCapture() == GetSafeHwnd())
		{
			ReleaseCapture();
		}

		CRect rectClient;
		GetClientRect(rectClient);

		mLeftColumnWidth = std::min<int>(
			std::max<int>(mRowHeight, point.x),
			rectClient.Width() - GetSystemMetrics(SM_CXHSCROLL) - 5);

		HDITEM hdItem;
		hdItem.mask = HDI_WIDTH ;
		hdItem.cxy = mLeftColumnWidth + 2;

		GetHeaderCtrl().SetItem(0, &hdItem);

		hdItem.cxy = rectClient.Width() + 10;
		GetHeaderCtrl().SetItem(1, &hdItem);
	}

	if (mTrackingDescr)
	{
		TrackDescr(INT_MIN);
		mTrackingDescr = FALSE;

		if (::GetCapture() == GetSafeHwnd())
		{
			ReleaseCapture();
		}

		CRect rectClient;
		GetClientRect(rectClient);

		point.y = std::max<int>(point.y, mRowHeight + mHeaderHeight);
		mDescrHeight = rectClient.Height() - point.y + 2;
		mDescrHeight = max(mRowHeight, mDescrHeight);

		AdjustLayout();
		RedrawWindow();
	}
}

void PropGridCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	if (GetStyle() & WS_BORDER)
	{
		lpncsp->rgrc[0].left++;
		lpncsp->rgrc[0].top++ ;
		lpncsp->rgrc[0].right--;
		lpncsp->rgrc[0].bottom--;
	}
}

void PropGridCtrl::OnNcPaint()
{
	if (mDescrHeight == -1)
	{
		mDescrHeight = mRowHeight * mDescrRows + mRowHeight / 2;
		AdjustLayout();
	}

	if (GetStyle() & WS_BORDER)
	{
		theVisualManager->OnDrawControlBorder(this);
	}
}

void PropGridCtrl::SetGroupNameFullWidth(BOOL bGroupNameFullWidth, BOOL bRedraw/* = TRUE*/)
{
	mGroupNameFullWidth = bGroupNameFullWidth;

	if (bRedraw && GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

void PropGridCtrl::SetCustomColors(
	COLORREF clrBackground,
	COLORREF clrText,
	COLORREF clrGroupBackground,
	COLORREF clrGroupText,
	COLORREF clrDescriptionBackground,
	COLORREF clrDescriptionText,
	COLORREF clrLine)
{
	mBackgroundColor = clrBackground;
	mTextColor = clrText;
	mGroupBackgroundColor = clrGroupBackground;
	mGroupTextColor = clrGroupText;
	mDescriptionBackgroundColor = clrDescriptionBackground;
	mDescriptionTextColor = clrDescriptionText;
	mLineColor = clrLine;

	mBackgroundBrush.DeleteObject();

	if (mBackgroundColor != (COLORREF)-1)
	{
		mBackgroundBrush.CreateSolidBrush(mBackgroundColor);
	}
}

void PropGridCtrl::GetCustomColors(
	COLORREF& clrBackground,
	COLORREF& clrText,
	COLORREF& clrGroupBackground,
	COLORREF& clrGroupText,
	COLORREF& clrDescriptionBackground,
	COLORREF& clrDescriptionText,
	COLORREF& clrLine)
{
	clrBackground = mBackgroundColor;
	clrText = mTextColor;
	clrGroupBackground = mGroupBackgroundColor;
	clrGroupText = mGroupTextColor;
	clrDescriptionBackground = mDescriptionBackgroundColor;
	clrDescriptionText = mDescriptionTextColor;
	clrLine = mLineColor;
}

BOOL PropGridCtrl::ProcessClipboardAccelerators(UINT nChar)
{
	if (mSelection == nullptr || mSelection->mWndInPlace->GetSafeHwnd() == nullptr)
	{
		return FALSE;
	}

	ASSERT_VALID(mSelection);

	BOOL bIsCtrl = (::GetAsyncKeyState(VK_CONTROL) & 0x8000);
	BOOL bIsShift = (::GetAsyncKeyState(VK_SHIFT) & 0x8000);

	if (bIsCtrl &&(nChar == _T('C') || nChar == VK_INSERT))
	{
		mSelection->mWndInPlace->SendMessageW(WM_COPY);
		return TRUE;
	}

	if (bIsCtrl && nChar == _T('V') ||(bIsShift && nChar == VK_INSERT))
	{
		mSelection->mWndInPlace->SendMessageW(WM_PASTE);
		return TRUE;
	}

	if (bIsCtrl && nChar == _T('X') ||(bIsShift && nChar == VK_DELETE))
	{
		mSelection->mWndInPlace->SendMessageW(WM_CUT);
		return TRUE;
	}

	return FALSE;
}

int PropGridCtrl::CompareProps(const PropGridProperty* pProp1, const PropGridProperty* pProp2) const
{
	ASSERT_VALID(this);
	ASSERT_VALID(pProp1);
	ASSERT_VALID(pProp2);

	return pProp1->mName.Compare(pProp2->mName);
}

LRESULT PropGridCtrl::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (GetGlobalData()->IsAccessibilitySupport())
	{
		return CWnd::OnGetObject(wParam, lParam);
	}

	return(LRESULT)0L;
}

void PropGridCtrl::NotifyAccessibility(PropGridProperty* pProp)
{
	if (!GetGlobalData()->IsAccessibilitySupport() || pProp == nullptr)
	{
		return;
	}

	accProp = pProp;

	CPoint pt(pProp->mRect.left, pProp->mRect.top);
	ClientToScreen(&pt);
	LPARAM lParam = MAKELPARAM(pt.x, pt.y);

	::NotifyWinEvent (EVENT_OBJECT_FOCUS, GetSafeHwnd(), OBJID_CLIENT, (LONG)lParam);
}

void PropGridCtrl::MarkModifiedProperties(BOOL bMark/* = TRUE*/, BOOL bRedraw/* = TRUE*/)
{
	mMarkModifiedProperties = bMark;

	if (bRedraw && GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

void PropGridCtrl::ResetOriginalValues(BOOL bRedraw)
{
	for (POSITION pos = mProps.GetHeadPosition(); pos != nullptr; /**/)
	{
		PropGridProperty* pProp = mProps.GetNext(pos);
		ASSERT_VALID(pProp);

		pProp->ResetOriginalValue();
	}

	if (bRedraw && GetSafeHwnd() != nullptr)
	{
		RedrawWindow();
	}
}

HRESULT PropGridCtrl::get_accChildCount(long *pcountChildren)
{
	if (!pcountChildren)
	{
		return E_INVALIDARG;
	}

	*pcountChildren = 0;
	return S_OK;
}

HRESULT PropGridCtrl::get_accChild(VARIANT /*varChild*/, IDispatch **ppdispChild)
{
	if (!(*ppdispChild))
	{
		return E_INVALIDARG;
	}

	if (m_pStdObject != nullptr)
	{
		*ppdispChild = m_pStdObject;
	}
	else
	{
		*ppdispChild = nullptr;
	}
	return S_OK;
}

HRESULT PropGridCtrl::get_accName(VARIANT varChild, BSTR *pszName)
{
	if (pszName == nullptr)
	{
		return E_INVALIDARG;
	}

	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		CString strText;
		GetWindowText(strText);
		if (strText.GetLength() == 0)
		{
			*pszName  = SysAllocString(L"PropertyList");
			return S_OK;
		}

		*pszName = strText.AllocSysString();
		return S_OK;
	}

	if (accProp != nullptr)
	{
		CString strName = accProp->IsInPlaceEditing() ? accProp->FormatProperty() : accProp->GetName();
		*pszName = strName.AllocSysString();
		return S_OK;
	}

	return S_OK;
}

HRESULT PropGridCtrl::get_accValue(VARIANT varChild, BSTR *pszValue)
{
	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		return S_FALSE;
	}

	if (accProp != nullptr)
	{
		BOOL bGroup = (accProp->IsGroup() && !accProp->mIsValueList);
		if (!bGroup)
		{
			CString strValue = accProp->FormatProperty();
			*pszValue = strValue.AllocSysString();
			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT PropGridCtrl::get_accDescription(VARIANT varChild, BSTR* pszDescription)
{
	if (((varChild.vt != VT_I4) && (varChild.lVal != CHILDID_SELF)) || (nullptr == pszDescription))
	{
		return E_INVALIDARG;
	}

	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		*pszDescription = SysAllocString(L"PropertyList");
		return S_OK;
	}

	if (accProp != nullptr)
	{
		CString strName = accProp->GetName();
		*pszDescription = strName.AllocSysString();
	}

	return S_OK;
}

HRESULT PropGridCtrl::get_accRole(VARIANT varChild, VARIANT *pvarRole)
{
	if (!pvarRole || ((varChild.vt != VT_I4) && (varChild.lVal != CHILDID_SELF)))
	{
		return E_INVALIDARG;
	}

	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_LIST;
		return S_OK;
	}

	pvarRole->vt = VT_I4;
	pvarRole->lVal = ROLE_SYSTEM_ROW;

	return S_OK;
}

HRESULT PropGridCtrl::get_accState(VARIANT varChild, VARIANT *pvarState)
{
	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		pvarState->vt = VT_I4;
		pvarState->lVal = STATE_SYSTEM_NORMAL;
		return S_OK;
	}

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_FOCUSABLE;
	pvarState->lVal |= STATE_SYSTEM_SELECTABLE;
	
	if (accProp != nullptr)
	{
		if (accProp->IsSelected())
		{
			pvarState->lVal |= STATE_SYSTEM_FOCUSED;
			pvarState->lVal |= STATE_SYSTEM_SELECTED;
		}

		BOOL bGroup = (accProp->IsGroup() && !accProp->mIsValueList);
		if (!accProp->IsEnabled() || bGroup)
		{
			pvarState->lVal |= STATE_SYSTEM_READONLY;
		}
	}

	return S_OK;
}

HRESULT PropGridCtrl::get_accHelp(VARIANT /*varChild*/, BSTR * /*pszHelp*/)
{
	return S_FALSE;
}

HRESULT PropGridCtrl::get_accHelpTopic(BSTR * /*pszHelpFile*/, VARIANT /*varChild*/, long * /*pidTopic*/)
{
	return S_FALSE;
}

HRESULT PropGridCtrl::get_accKeyboardShortcut(VARIANT /*varChild*/, BSTR* /*pszKeyboardShortcut*/)
{
	return S_FALSE;
}

HRESULT PropGridCtrl::get_accFocus(VARIANT* pvarChild)
{
	if (nullptr == pvarChild)
	{
		return E_INVALIDARG;
	}

	return DISP_E_MEMBERNOTFOUND;
}

HRESULT PropGridCtrl::get_accSelection(VARIANT* pvarChildren)
{
	if (nullptr == pvarChildren)
	{
		return E_INVALIDARG;
	}
	return DISP_E_MEMBERNOTFOUND;
}

HRESULT PropGridCtrl::get_accDefaultAction(VARIANT /*varChild*/, BSTR* /*pszDefaultAction*/)
{
	return DISP_E_MEMBERNOTFOUND; 
}

HRESULT PropGridCtrl::accSelect(long flagsSelect, VARIANT varChild)
{
	if (m_pStdObject != nullptr)
	{
		return m_pStdObject->accSelect(flagsSelect, varChild);
	}
	return E_INVALIDARG;
}

HRESULT PropGridCtrl::accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
{
	HRESULT hr = S_OK;

	if (!pxLeft || !pyTop || !pcxWidth || !pcyHeight)
	{
		return E_INVALIDARG;
	}

	if ((varChild.vt == VT_I4) && (varChild.lVal == CHILDID_SELF))
	{
		CRect rc;
		GetWindowRect(rc);

		*pxLeft = rc.left;
		*pyTop = rc.top;
		*pcxWidth = rc.Width();
		*pcyHeight = rc.Height();

		return S_OK;
	}
	else
	{
		if (accProp != nullptr)
		{
			CRect rcProp = accProp->mRect;
			ClientToScreen(&rcProp);
			*pxLeft = rcProp.left;
			*pyTop = rcProp.top;
			*pcxWidth = rcProp.Width();
			*pcyHeight = rcProp.Height();
		}
	}

	return hr;
}

HRESULT PropGridCtrl::accHitTest(long  xLeft, long yTop, VARIANT* pvarChild)
{
	if (!pvarChild)
	{
		return E_INVALIDARG;
	}

	CPoint pt(xLeft, yTop);
	ScreenToClient(&pt);

	PropGridProperty* pProp = HitTest(pt);
	if (pProp != nullptr)
	{
		LPARAM lParam = MAKELPARAM((WORD)xLeft, (WORD)yTop);
		pvarChild->vt = VT_I4;
		pvarChild->lVal = (LONG)lParam;
	}
	else
	{
		pvarChild->vt = VT_I4;
		pvarChild->lVal = CHILDID_SELF;
	}

	accProp = pProp;
	return S_OK;
}

LRESULT PropGridCtrl::OnInitControl(WPARAM wParam, LPARAM lParam)
{
	DWORD dwSize = (DWORD)wParam;
	BYTE* pbInitData = (BYTE*)lParam;

	CString strDst;
	CMFCControlContainer::UTF8ToString((LPSTR)pbInitData, strDst, dwSize);

	CTagManager tagManager(strDst);

	BOOL bDescriptionArea = TRUE;
	if (ReadBoolProp(tagManager, PS_MFCPropertyGrid_DescriptionArea, bDescriptionArea))
	{
		EnableDescriptionArea(bDescriptionArea);
	}

	CString strDescriptionRows;
	if (tagManager.ExcludeTag(PS_MFCPropertyGrid_DescriptionRows, strDescriptionRows))
	{
		if (!strDescriptionRows.IsEmpty())
		{
			int nDescriptionRows = _ttoi((LPCTSTR)strDescriptionRows);
			if (nDescriptionRows >= 0)
			{
				SetDescriptionRows(nDescriptionRows);
			}
		}
	}

	BOOL bHeaderCtrl = TRUE;
	if (ReadBoolProp(tagManager, PS_MFCPropertyGrid_HeaderCtrl, bHeaderCtrl))
	{
		EnableHeaderCtrl(bHeaderCtrl);
	}

	BOOL bAlphabeticMode = FALSE;
	if (ReadBoolProp(tagManager, PS_MFCPropertyGrid_AlphabeticMode, bAlphabeticMode))
	{
		SetAlphabeticMode(bAlphabeticMode);
	}

	BOOL bModifiedProperties = TRUE;
	if (ReadBoolProp(tagManager, PS_MFCPropertyGrid_ModifiedProperties, bModifiedProperties))
	{
		MarkModifiedProperties(bModifiedProperties);
	}

	BOOL bVSDotNetLook = TRUE;
	if (ReadBoolProp(tagManager, PS_MFCPropertyGrid_VSDotNetLook, bVSDotNetLook))
	{
		SetVSDotNetLook(bVSDotNetLook);
	}

	return 0;
}

LRESULT PropGridCtrl::OnPrintClient(CDC* pDC, UINT nFlags)
{
	ASSERT_VALID(pDC);

	if (nFlags & PRF_CLIENT)
	{
		OnDraw(pDC);
	}

	return 0L;
}

PropGridCtrl::IMediator::~IMediator()
{
}

} // namespace hsui
