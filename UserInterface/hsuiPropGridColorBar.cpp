#include "hsuiPrecompiled.h"
#include "hsuiPropGridColorBar.h"
#include "hsuiPropGridCtrl.h"

#undef max
#undef min

namespace hsui {

static const int PGCB_SEPARATOR_SIZE = 2;

IMPLEMENT_SERIAL(PropGridColorBar, CMFCPopupMenuBar, 1)

BEGIN_MESSAGE_MAP(PropGridColorBar, CMFCPopupMenuBar)
	ON_WM_CREATE()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

CMap<COLORREF, COLORREF, CString, LPCTSTR> PropGridColorBar::msColorNames;

PropGridColorBar::PropGridColorBar()
{
	mNumColumns = 0;
	mNumRowsHorz = 0;
	mNumColumnsVert = 0;
	mBoxSize = CSize(0, 0);
	mRowHeight = 0;
	mColorSelected = (COLORREF) -1;
	mColorAutomatic = (COLORREF) -1;
	mCommandID = 0;
	mStdColorDlg = FALSE;
	mIsTearOff = TRUE;
	mShowDocColorsWhenDocked = FALSE;
	m_bLocked = TRUE;
	mIsEnabled = TRUE;
	mPropGridCtrl = nullptr;
	mHorzOffset = mVertOffset = 0;
	mInternal = FALSE;
	mVertMargin = 4;
	mHorzMargin = 4;
}

PropGridColorBar::PropGridColorBar(
	const CArray<COLORREF, COLORREF>& colors,
	COLORREF color,
	LPCTSTR autoColorString,
	LPCTSTR otherColorString,
	LPCTSTR docColorString,
	CList<COLORREF,COLORREF>& docColors,
	int numColumns,
	int numRowsDockHorz,
	int numColDockVert,
	COLORREF colorAutomatic,
	UINT commandID)
	:
	mColorSelected(color),
	mAutoColorString(autoColorString == nullptr ? _T("") : autoColorString),
	mOtherColorString(otherColorString == nullptr ? _T("") : otherColorString),
	mDocColorString(docColorString == nullptr ? _T("") : docColorString),
	mNumColumns(numColumns),
	mNumRowsHorz(numRowsDockHorz),
	mNumColumnsVert(numColDockVert),
	mColorAutomatic(colorAutomatic),
	mIsTearOff(FALSE),
	mStdColorDlg(FALSE),
	mCommandID(commandID)
{
	mPropGridCtrl = nullptr;

	mColors.SetSize(colors.GetSize());

	for (int i = 0; i < colors.GetSize(); i++)
	{
		mColors [i] = colors [i];
	}

	mDocColors.AddTail(&docColors);
	m_bLocked = TRUE;
	mIsEnabled = TRUE;
	mShowDocColorsWhenDocked = TRUE;

	mHorzOffset = mVertOffset = 0;
	mInternal = FALSE;
	mVertMargin = 4;
	mHorzMargin = 4;
}

PropGridColorBar::PropGridColorBar(PropGridColorBar& src, UINT uiCommandID)
	:
	mColorSelected(src.mColorSelected),
	mAutoColorString(src.mAutoColorString),
	mOtherColorString(src.mOtherColorString),
	mDocColorString(src.mDocColorString),
	mColorAutomatic(src.mColorAutomatic),
	mNumColumns(src.mNumColumns),
	mNumRowsHorz(src.mNumRowsHorz),
	mNumColumnsVert(src.mNumColumnsVert),
	mIsTearOff(TRUE),
	mCommandID(uiCommandID),
	mStdColorDlg(src.mStdColorDlg)
{
	mColors.SetSize(src.mColors.GetSize());

	for (int i = 0; i < src.mColors.GetSize(); i++)
	{
		mColors [i] = src.mColors [i];
	}

	mDocColors.AddTail(&src.mDocColors);
	m_bLocked = TRUE;
	mIsEnabled = TRUE;
	mShowDocColorsWhenDocked = FALSE;
	mPropGridCtrl = nullptr;
	mHorzOffset = mVertOffset = 0;
	mInternal = FALSE;
	mVertMargin = 4;
	mHorzMargin = 4;
}

PropGridColorBar::~PropGridColorBar()
{
}

void PropGridColorBar::AdjustLocations()
{
	if (GetSafeHwnd() == nullptr || !::IsWindow(m_hWnd) || m_bInUpdateShadow)
	{
		return;
	}

	ASSERT_VALID(this);

	CRect rectClient; // Client area rectangle
	GetClientRect(&rectClient);

	rectClient.DeflateRect(mHorzMargin + mHorzOffset, mVertMargin + mVertOffset);

	int x = rectClient.left;
	int y = rectClient.top;
	int i = 0;

	BOOL bPrevSeparator = FALSE;
	BOOL bIsOtherColor = (mAutoColorString.IsEmpty() || mColorSelected != (COLORREF)-1);

	for (POSITION pos = m_Buttons.GetHeadPosition(); pos != nullptr; i ++)
	{
		CRect rectButton(0, 0, 0, 0);

		CMFCToolBarButton* pButton = (CMFCToolBarButton*) m_Buttons.GetNext(pos);
		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			if (bPrevSeparator)
			{
				rectButton.SetRectEmpty();
			}
			else
			{
				if (x > rectClient.left)
				{
					// Next line
					x = rectClient.left;
					y += mBoxSize.cy + mVertMargin;
				}

				rectButton = CRect(CPoint(x, y), CSize(rectClient.Width(), PGCB_SEPARATOR_SIZE));

				y += PGCB_SEPARATOR_SIZE + 2;
				x = rectClient.left;
			}

			bPrevSeparator = TRUE;
		}
		else
		{
			PropGridColorButton* pColorButton = DYNAMIC_DOWNCAST(PropGridColorButton, pButton);
			if (pColorButton == nullptr)
			{
				continue;
			}

			ASSERT_VALID(pColorButton);

			if (pColorButton->mIsDocument && !mShowDocColorsWhenDocked && !IsFloating())
			{
				rectButton.SetRectEmpty();
			}
			else if (
				pColorButton->mIsAutomatic ||
				pColorButton->mIsOther ||
				pColorButton->mIsLabel)
			{
				if (x > rectClient.left)
				{
					// Next line
					x = rectClient.left;
					y += mBoxSize.cy + mVertMargin;
				}

				if (pColorButton->mIsOther && bIsOtherColor)
				{
					rectButton = CRect(CPoint(x, y),
						CSize(rectClient.Width() - mBoxSize.cx,
							  mRowHeight - mVertMargin / 2));

					x = rectButton.right;
					y += (rectButton.Height() - mBoxSize.cy) / 2;
				}
				else
				{
					rectButton = CRect(CPoint(x, y),
						CSize(rectClient.Width(), mRowHeight - mVertMargin / 2));
					y += mRowHeight - mVertMargin / 2;
					x = rectClient.left;
				}

				if (pColorButton->mIsOther)
				{
					rectButton.DeflateRect(mHorzMargin / 2, mVertMargin / 2);
				}

				bPrevSeparator = FALSE;
			}
			else
			{
				if (x + mBoxSize.cx > rectClient.right)
				{
					x = rectClient.left;
					y += mBoxSize.cy;
				}

				if (pColorButton->mIsOtherColor && !bIsOtherColor)
				{
					rectButton.SetRectEmpty();
				}
				else
				{
					rectButton = CRect(CPoint(x, y), mBoxSize);
					x += mBoxSize.cx;

					bPrevSeparator = FALSE;
				}

				if (pColorButton->mColor == mColorSelected && !pColorButton->mIsOtherColor)
				{
					bIsOtherColor = FALSE;
				}
			}
		}

		pButton->SetRect(rectButton);
	}

	UpdateTooltips();
}

CSize PropGridColorBar::CalcSize(BOOL bVertDock)
{
	CSize sizeGrid = GetColorGridSize(bVertDock);

	return CSize(sizeGrid.cx * mBoxSize.cx + 2 * mVertMargin,
		sizeGrid.cy * mBoxSize.cy + GetExtraHeight(sizeGrid.cx) + 2 * mHorzMargin);
}

CSize PropGridColorBar::GetColorGridSize(BOOL bVertDock) const
{
	// Calculate number of columns and rows in the color grid

	int nNumColumns = 0;
	int nNumRows = 0;

	int nColors = (int) mColors.GetSize();

	if (!mIsTearOff || IsFloating() || bVertDock || mNumRowsHorz <= 0)
	{
		nNumColumns =
			!mIsTearOff ||
			IsFloating() ||
			mNumColumnsVert <= 0 ? mNumColumns : mNumColumnsVert;
		if (nNumColumns <= 0)
		{
			nNumColumns = (int)(sqrt((double) nColors)) + 1;
		}

		nNumRows = nColors / nNumColumns;
		if ((nColors % nNumColumns) != 0)
		{
			nNumRows ++;
		}
	}
	else // Horz dock
	{
		nNumRows = mNumRowsHorz;
		nNumColumns = nColors / nNumRows;

		if ((nColors % nNumRows) != 0)
		{
			nNumColumns ++;
		}
	}

	return CSize(nNumColumns, nNumRows);
}

int PropGridColorBar::GetExtraHeight(int nNumColumns) const
{
	// Calculate additional height required by the misc. elements such
	// as "Other" button, document colors, e.t.c

	int nExtraHeight = 0;

	if (!mAutoColorString.IsEmpty())
	{
		nExtraHeight += mRowHeight;
	}
	else if (!mOtherColorString.IsEmpty())
	{
		nExtraHeight += mVertMargin;
	}

	if (!mOtherColorString.IsEmpty())
	{
		nExtraHeight += mRowHeight;
	}

	if (!mDocColorString.IsEmpty() &&
		!mDocColors.IsEmpty() &&
		(mShowDocColorsWhenDocked || IsFloating()))
	{
		int nDocColorRows = (int) mDocColors.GetCount() / nNumColumns;
		if ((mDocColors.GetCount() % nNumColumns) != 0)
		{
			nDocColorRows++;
		}

		nExtraHeight += mRowHeight +
			nDocColorRows * mBoxSize.cy +
			2 * PGCB_SEPARATOR_SIZE + mVertMargin;
	}

	return nExtraHeight;
}

void PropGridColorBar::SetDocumentColors(LPCTSTR lpszCaption,
	CList<COLORREF,COLORREF>& lstDocColors, BOOL bShowWhenDocked)
{
	mDocColorString = lpszCaption == nullptr ? _T("") : lpszCaption;

	if (mDocColors.GetCount() == lstDocColors.GetCount())
	{
		BOOL bChanged = FALSE;

		POSITION posCur = mDocColors.GetHeadPosition();
		POSITION posNew = lstDocColors.GetHeadPosition();

		while (posCur != nullptr && posNew != nullptr)
		{
			if (mDocColors.GetNext(posCur) != lstDocColors.GetNext(posNew))
			{
				bChanged = TRUE;
				break;
			}
		}

		if (!bChanged)
		{
			return;
		}
	}

	mDocColors.RemoveAll();
	mDocColors.AddTail(&lstDocColors);

	mShowDocColorsWhenDocked = bShowWhenDocked;

	Rebuild();
	AdjustLayout();
}

void PropGridColorBar::ContextToSize(BOOL bSquareButtons, BOOL bCenterButtons)
{
	ENSURE(GetSafeHwnd() != nullptr);

	CRect rectClient;
	GetClientRect(rectClient);

	// First, adjust height:
	int nCurrHeight = CalcSize(TRUE).cy;
	int yDelta = nCurrHeight < rectClient.Height() ? 1 : -1;

	while ((nCurrHeight = CalcSize(TRUE).cy) != rectClient.Height())
	{
		if (yDelta < 0)
		{
			if (nCurrHeight < rectClient.Height())
			{
				break;
			}
		}
		else if (nCurrHeight > rectClient.Height())
		{
			mBoxSize.cy -= yDelta;
			mRowHeight = mBoxSize.cy * 3 / 2;
			break;
		}

		mBoxSize.cy += yDelta;
		mRowHeight = mBoxSize.cy * 3 / 2;
	}

	// Now, adjust width:
	int nCurrWidth = CalcSize(TRUE).cx;
	int xDelta = nCurrWidth < rectClient.Width() ? 1 : -1;

	while ((nCurrWidth = CalcSize(TRUE).cx) != rectClient.Width())
	{
		if (xDelta < 0)
		{
			if (nCurrWidth < rectClient.Width())
			{
				break;
			}
		}
		else if (nCurrWidth > rectClient.Width())
		{
			mBoxSize.cy -= xDelta;
			break;
		}

		mBoxSize.cx += xDelta;
	}

	mBoxSize.cx--;
	mBoxSize.cy--;

	if (bSquareButtons)
	{
		mBoxSize.cx = mBoxSize.cy = std::min(mBoxSize.cx, mBoxSize.cy);
		mRowHeight = mBoxSize.cy * 3 / 2;
	}

	if (bCenterButtons)
	{
		// Finaly, calculate offset to center buttons area:
		CSize size = CalcSize(TRUE);

		mHorzOffset = (rectClient.Width() - size.cx) / 2;
		mVertOffset = (rectClient.Height() - size.cy) / 2;
	}
	else
	{
		mHorzOffset = mVertOffset = 0;
	}

	AdjustLocations();
}

void PropGridColorBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	if (mIsTearOff)
	{
		CMFCToolBar::OnNcCalcSize(bCalcValidRects, lpncsp);
	}
	else
	{
		CMFCPopupMenuBar::OnNcCalcSize(bCalcValidRects, lpncsp);
	}
}

void PropGridColorBar::OnNcPaint()
{
	if (mIsTearOff)
	{
		CMFCToolBar::OnNcPaint();
	}
	else
	{
		CMFCPopupMenuBar::OnNcPaint();
	}
}

int PropGridColorBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPopupMenuBar::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	mBoxSize = GetMenuImageSize();
	mBoxSize.cx ++;
	mBoxSize.cy ++;

	m_bLeaveFocus = FALSE;
	mRowHeight = mBoxSize.cy * 3 / 2;
	Rebuild();

	if (mPropGridCtrl != nullptr)
	{
		SetCapture();
	}

	return 0;
}

void PropGridColorBar::Rebuild()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	RemoveAllButtons();

	BOOL bAlreadySelected = FALSE;
	if (!mAutoColorString.IsEmpty()) // Automatic
	{
		InsertButton(new PropGridColorButton(mColorAutomatic,
			TRUE, FALSE, mAutoColorString, mColorSelected == (COLORREF)-1));

		if (!bAlreadySelected)
		{
			bAlreadySelected = (mColorSelected == (COLORREF)-1);
		}
	}

	for (int i = 0; i < mColors.GetSize(); i ++)
	{
		InsertButton(new PropGridColorButton(mColors [i],
			FALSE, FALSE, nullptr, mColorSelected == mColors[i]));

		if (!bAlreadySelected)
		{
			bAlreadySelected = (mColorSelected == mColors[i]);
		}
	}

	if (!mDocColorString.IsEmpty() && !mDocColors.IsEmpty())
	{
		InsertSeparator();
		InsertButton(new PropGridColorButton(mDocColorString, TRUE)); // Label

		for (POSITION pos = mDocColors.GetHeadPosition(); pos != nullptr; /**/)
		{
			COLORREF color = mDocColors.GetNext(pos);
			InsertButton(new PropGridColorButton(color, FALSE, FALSE,
				nullptr, !bAlreadySelected && mColorSelected == color, TRUE));
		}
	}

	if (!mOtherColorString.IsEmpty()) // Other color button
	{
		InsertSeparator();
		InsertButton(new PropGridColorButton((COLORREF)-1, FALSE, TRUE, mOtherColorString));
		InsertButton(new PropGridColorButton(mColorSelected,
			FALSE, FALSE, nullptr, !bAlreadySelected, FALSE, TRUE));
	}
}

class ColorBarCmdUI : public CCmdUI
{
public:
	ColorBarCmdUI();

public: // re-implementations only
	virtual void Enable(BOOL bOn);
	BOOL mEnabled;
};

ColorBarCmdUI::ColorBarCmdUI()
{
	mEnabled = TRUE;  // assume it is enabled
}

void ColorBarCmdUI::Enable(BOOL bOn)
{
	mEnabled = bOn;
	m_bEnableChanged = TRUE;
}

void PropGridColorBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	ASSERT_VALID(this);

	if (mCommandID == 0 || mCommandID == (UINT)-1)
	{
		CMFCPopupMenuBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);
		return;
	}

	ColorBarCmdUI state;
	state.m_pOther = this;
	state.m_nIndexMax = 1;
	state.m_nID = mCommandID;

	BOOL bIsEnabled = FALSE;
	if (pTarget->OnCmdMsg(mCommandID, CN_UPDATE_COMMAND_UI, &state, nullptr))
	{
		bIsEnabled = state.mEnabled;
	}
	else if (bDisableIfNoHndler && !state.m_bEnableChanged)
	{
		AFX_CMDHANDLERINFO info;
		info.pTarget = nullptr;

		bIsEnabled = pTarget->OnCmdMsg(mCommandID, CN_COMMAND, &state, &info);
	}

	if (bIsEnabled != mIsEnabled)
	{
		mIsEnabled = bIsEnabled;

		for (POSITION pos = m_Buttons.GetHeadPosition(); pos != nullptr; /**/)
		{
			PropGridColorButton* pColorButton = DYNAMIC_DOWNCAST(PropGridColorButton, m_Buttons.GetNext(pos));
			if (pColorButton != nullptr)
			{
				pColorButton->m_nStyle &= ~TBBS_DISABLED;
				if (!bIsEnabled)
				{
					pColorButton->m_nStyle |= TBBS_DISABLED;
				}
			}
		}

		Invalidate();
		UpdateWindow();
	}

	CMFCPopupMenuBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);
}

void PropGridColorBar::DoPaint(CDC* pDC)
{
	CPalette* pOldPalette = SelectPalette(pDC);

	CMFCPopupMenuBar::DoPaint(pDC);

	if (pOldPalette != nullptr)
	{
		pDC->SelectPalette(pOldPalette, FALSE);
	}
}

BOOL PropGridColorBar::OnQueryNewPalette()
{
	Invalidate();
	UpdateWindow();
	return CMFCPopupMenuBar::OnQueryNewPalette();
}

void PropGridColorBar::OnPaletteChanged(CWnd* pFocusWnd)
{
	CMFCPopupMenuBar::OnPaletteChanged(pFocusWnd);

	if (pFocusWnd->GetSafeHwnd() != GetSafeHwnd())
	{
		Invalidate();
		UpdateWindow();
	}
}

BOOL PropGridColorBar::OnSendCommand(const CMFCToolBarButton* pButton)
{
	if (mPropGridCtrl != nullptr)
	{
		ReleaseCapture();
	}

	COLORREF color = (COLORREF) -1;

	CMFCColorMenuButton* pColorMenuButton = nullptr;

	CMFCPopupMenu* pParentMenu = DYNAMIC_DOWNCAST(CMFCPopupMenu, GetParent());
	if (pParentMenu != nullptr)
	{
		pColorMenuButton = DYNAMIC_DOWNCAST(CMFCColorMenuButton, pParentMenu->GetParentButton());
	}

	PropGridColorButton* pColorButton = DYNAMIC_DOWNCAST(PropGridColorButton, pButton);
	if (pColorButton == nullptr)
	{
		ASSERT(FALSE);
	}
	else if (pColorButton->mIsLabel)
	{
		return FALSE;
	}
	else if (pColorButton->mIsOther)
	{
		SetInCommand();

		if (pParentMenu != nullptr)
		{
			pParentMenu->ShowWindow(SW_HIDE);

			CFrameWnd* topLevelFrame = GetTopLevelFrame();
			if (topLevelFrame != nullptr)
			{
				CMFCPopupMenu::ActivatePopupMenu(topLevelFrame, nullptr);
			}
		}

		HWND hwnd = GetSafeHwnd();

		InvalidateRect(pButton->Rect());
		UpdateWindow();

		// Invoke color dialog:
		if (!OpenColorDialog(mColorSelected == (COLORREF)-1 ?
							 mColorAutomatic : mColorSelected, color))
		{
			if (!::IsWindow(hwnd))
			{
				return TRUE;
			}

			SetInCommand(FALSE);

			if (mPropGridCtrl != nullptr)
			{
				GetParent()->SendMessageW(WM_CLOSE);
			}
			else if (pColorMenuButton != nullptr)
			{
				InvokeMenuCommand(0, pColorMenuButton);
			}
			else if (GetTopLevelFrame() != nullptr)
			{
				GetTopLevelFrame()->SetFocus();
			}

			return TRUE;
		}

		if (!::IsWindow(hwnd))
		{
			return TRUE;
		}

		SetInCommand(FALSE);
	}
	else if (pColorButton->mIsAutomatic)
	{
		color = (COLORREF) -1;
	}
	else
	{
		color = pColorButton->mColor;
	}

	if (pColorMenuButton != nullptr)
	{
		pColorMenuButton->SetColor(color);
		InvokeMenuCommand(pColorMenuButton->m_nID, pColorMenuButton);
	}
	else if (mPropGridCtrl != nullptr)
	{
		mPropGridCtrl->UpdateColor(color);
		GetParent()->SendMessageW(WM_CLOSE);
	}
	else
	{
		ASSERT(mCommandID != 0);

		SetColor(color);

		CObList listButtons;
		if (CMFCToolBar::GetCommandButtons(mCommandID, listButtons) > 0)
		{
			for (POSITION pos = listButtons.GetHeadPosition(); pos != nullptr;)
			{
				CMFCColorMenuButton* pCurrColorButton = nullptr;
				pCurrColorButton = DYNAMIC_DOWNCAST(CMFCColorMenuButton, listButtons.GetNext(pos));
				if (pCurrColorButton != nullptr)
				{
					ASSERT_VALID(pCurrColorButton);
					pCurrColorButton->SetColor(color, FALSE);
				}
			}
		}

		CMFCColorMenuButton::SetColorByCmdID(mCommandID, color);
		GetOwner()->SendMessageW(WM_COMMAND, mCommandID);    // send command

		CFrameWnd* topLevelFrame = GetTopLevelFrame();
		if (topLevelFrame != nullptr)
		{
			topLevelFrame->SetFocus();
		}
	}

	return TRUE;
}

BOOL PropGridColorBar::Create(CWnd* parentWnd, DWORD style, UINT id,
	CPalette* palette, int numColumns, int numRowsDockHorz, int numColDockVert)
{
	if (mColors.GetSize() != 0)
	{
		return CMFCPopupMenuBar::Create(parentWnd, style, id);
	}

	mNumColumns = numColumns;
	mNumColumnsVert = numColDockVert;
	mNumRowsHorz = numRowsDockHorz;

	InitColors(palette, mColors);

	return CMFCPopupMenuBar::Create(parentWnd, style, id);
}

BOOL PropGridColorBar::CreateControl(CWnd* parentWnd, const CRect& rect, UINT id, int numColumns, CPalette* palette)
{
	ASSERT_VALID(parentWnd);

	EnableLargeIcons(FALSE);

	if (numColumns <= 0)
	{
		const int colorsCount = (palette == nullptr) ? 20 : palette->GetEntryCount();
		ASSERT(colorsCount > 0);

		// Optimal fill
		for (numColumns = colorsCount; numColumns > 0; numColumns--)
		{
			int cellSize = (rect.Width() - 2 * mHorzMargin) / numColumns;
			if (cellSize == 0)
			{
				continue;
			}

			int numRows = colorsCount / numColumns;
			if (numRows * cellSize > rect.Height() - 2 * mVertMargin)
			{
				numColumns++;
				break;
			}
		}

		if (numColumns <= 0)
		{
			numColumns = -1;
		}
	}

	if (!Create(parentWnd, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, id, palette, numColumns))
	{
		return FALSE;
	}

	SetPaneStyle(GetPaneStyle() &
		~(CBRS_GRIPPER |
		  CBRS_BORDER_TOP |
		  CBRS_BORDER_BOTTOM |
		  CBRS_BORDER_LEFT |
		  CBRS_BORDER_RIGHT));

	CRect rectWnd = rect;
	MoveWindow(rectWnd);
	ContextToSize();

	SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	SetOwner(parentWnd);
	SetCommandID(id);

	// All commands will be routed via this dialog, not via the parent frame:
	SetRouteCommandsViaFrame(FALSE);
	return TRUE;
}

int __stdcall PropGridColorBar::InitColors(CPalette* palette, CArray<COLORREF, COLORREF>& colors)
{
	int colorsCount = (palette == nullptr) ? 20 : palette->GetEntryCount();
	colors.SetSize(colorsCount);

	if (palette == nullptr)
	{
		// Use system palette:
		palette = CPalette::FromHandle(static_cast<HPALETTE>(::GetStockObject(DEFAULT_PALETTE)));
		ASSERT_VALID(palette);
	}

	PALETTEENTRY palEntry;
	for (int i = 0; i < colorsCount; i++)
	{
		palette->GetPaletteEntries(i, 1, &palEntry);
		colors[i] = RGB(palEntry.peRed, palEntry.peGreen, palEntry.peBlue);
	}

	return colorsCount;
}

void PropGridColorBar::Serialize(CArchive& ar)
{
	CMFCPopupMenuBar::Serialize(ar);

	if (ar.IsLoading())
	{
		ar >> mNumColumns;
		ar >> mNumRowsHorz;
		ar >> mNumColumnsVert;
		ar >> mColorAutomatic;
		ar >> mAutoColorString;
		ar >> mOtherColorString;
		ar >> mDocColorString;
		ar >> mIsTearOff;
		ar >> mCommandID;
		ar >> mStdColorDlg;

		int nColors = 0;
		ar >> nColors;

		mColors.SetSize(nColors);

		for (int i = 0; i < nColors; i ++)
		{
			COLORREF color;
			ar >> color;

			mColors [i] = color;
		}

		Rebuild();
		AdjustLocations();
	}
	else
	{
		ar << mNumColumns;
		ar << mNumRowsHorz;
		ar << mNumColumnsVert;
		ar << mColorAutomatic;
		ar << mAutoColorString;
		ar << mOtherColorString;
		ar << mDocColorString;
		ar << mIsTearOff;
		ar << mCommandID;
		ar << mStdColorDlg;

		ar <<(int) mColors.GetSize();

		for (int i = 0; i < mColors.GetSize(); i ++)
		{
			ar << mColors [i];
		}
	}
}

void PropGridColorBar::ShowCommandMessageString(UINT /*uiCmdId*/)
{
	GetOwner()->SendMessageW(WM_SETMESSAGESTRING,
		mCommandID == (UINT) -1 ? AFX_IDS_IDLEMESSAGE :(WPARAM) mCommandID);
}

BOOL PropGridColorBar::OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes)
{
	CMFCColorMenuButton* pColorMenuButton = nullptr;

	CMFCPopupMenu* pParentMenu = DYNAMIC_DOWNCAST(CMFCPopupMenu, GetParent());
	if (pParentMenu != nullptr)
	{
		pColorMenuButton = DYNAMIC_DOWNCAST(CMFCColorMenuButton, pParentMenu->GetParentButton());
		if (pColorMenuButton != nullptr)
		{
			return pColorMenuButton->OpenColorDialog(colorDefault, colorRes);
		}
	}

	BOOL bResult = FALSE;

	if (mStdColorDlg)
	{
		CColorDialog dlg(colorDefault, CC_FULLOPEN | CC_ANYCOLOR);
		if (dlg.DoModal() == IDOK)
		{
			colorRes = dlg.GetColor();
			bResult = TRUE;
		}
	}
	else
	{
		CMFCColorDialog dlg(colorDefault);
		if (dlg.DoModal() == IDOK)
		{
			colorRes = dlg.GetColor();
			bResult = TRUE;
		}
	}

	return bResult;
}

void PropGridColorBar::EnableAutomaticButton(
	LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable)
{
	mColorAutomatic = colorAutomatic;
	mAutoColorString = (!bEnable || lpszLabel == nullptr) ? _T("") : lpszLabel;

	Rebuild();
	AdjustLayout();
}

void PropGridColorBar::EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg, BOOL bEnable)
{
	mStdColorDlg = !bAltColorDlg;
	mOtherColorString = (!bEnable || lpszLabel == nullptr) ? _T("") : lpszLabel;

	Rebuild();
	AdjustLayout();
}

void PropGridColorBar::SetColor(COLORREF color)
{
	if (mColorSelected == color)
	{
		return;
	}

	mColorSelected = color;

	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	BOOL bIsOtherColor = !(mColorAutomatic != (UINT)-1 && mColorSelected == (COLORREF)-1);
	BOOL bWasOtherColor = FALSE;

	m_iHighlighted = -1;
	int iButton = 0;

	for (POSITION pos = m_Buttons.GetHeadPosition(); pos != nullptr; iButton++)
	{
		CRect rectButton;

		CMFCToolBarButton* pButton = (CMFCToolBarButton*)m_Buttons.GetNext(pos);
		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			continue;
		}

		PropGridColorButton* pColorButton = DYNAMIC_DOWNCAST(PropGridColorButton, pButton);
		if (pColorButton == nullptr)
		{
			continue;
		}

		ASSERT_VALID(pColorButton);

		if (pColorButton->mIsOther || pColorButton->mIsLabel)
		{
			continue;
		}

		if (pColorButton->mHighlight)
		{
			pColorButton->mHighlight = FALSE;
			InvalidateRect(pColorButton->Rect());
		}

		if (pColorButton->mIsAutomatic && color == (COLORREF)-1)
		{
			pColorButton->mHighlight = TRUE;
			m_iHighlighted = iButton;
			InvalidateRect(pColorButton->Rect());
		}
		else if (pColorButton->mColor == color)
		{
			pColorButton->mHighlight = TRUE;
			m_iHighlighted = iButton;
			InvalidateRect(pColorButton->Rect());
			bIsOtherColor = FALSE;
		}

		if (pColorButton->mIsOtherColor)
		{
			pColorButton->mColor = mColorSelected;
			pColorButton->mHighlight = TRUE;

			InvalidateRect(pColorButton->Rect());
			bWasOtherColor = !(pColorButton->Rect().IsRectEmpty());
		}
	}

	if (bWasOtherColor != bIsOtherColor)
	{
		AdjustLocations();
		Invalidate();
	}

	UpdateWindow();
}

void PropGridColorBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!IsCustomizeMode() || mInternal)
	{
		CMFCToolBar::OnMouseMove(nFlags, point);
	}
}

void PropGridColorBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsCustomizeMode() && !mInternal)
	{
		return;
	}

	if (HitTest(point) == -1)
	{
		CMFCToolBar::OnLButtonDown(nFlags, point);
	}
}

void PropGridColorBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!IsCustomizeMode() || mInternal)
	{
		int iHit = HitTest(point);
		if (iHit >= 0)
		{
			m_iButtonCapture = iHit;
		}

		CMFCToolBar::OnLButtonUp(nFlags, point);
	}
}

void PropGridColorBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!IsCustomizeMode() || mInternal)
	{
		CMFCToolBar::OnLButtonDblClk(nFlags, point);
	}
}

BOOL PropGridColorBar::PreTranslateMessage(MSG* pMsg)
{
	if (mPropGridCtrl != nullptr && !m_bInCommand)
	{
		switch (pMsg->message)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			{
				CRect rect;
				GetClientRect(rect);

				CPoint pt(AFX_GET_X_LPARAM(pMsg->lParam), AFX_GET_Y_LPARAM(pMsg->lParam));
				if (!rect.PtInRect(pt))
				{
					GetParent()->SendMessageW(WM_CLOSE);
					return TRUE;
				}
			}
			break;

		case WM_SYSKEYDOWN:
		case WM_CONTEXTMENU:
			GetParent()->SendMessageW(WM_CLOSE);
			return TRUE;

		case WM_KEYDOWN:
			if (pMsg->wParam == VK_ESCAPE)
			{
				GetParent()->SendMessageW(WM_CLOSE);
				return TRUE;
			}
		}
	}

	return CMFCPopupMenuBar::PreTranslateMessage(pMsg);
}

void PropGridColorBar::OnDestroy()
{
	if (mPropGridCtrl != nullptr)
	{
		mPropGridCtrl->CloseColorPopup();
		mPropGridCtrl->SetFocus();
	}

	CMFCPopupMenuBar::OnDestroy();
}

BOOL PropGridColorBar::OnKey(UINT nChar)
{
	POSITION posSel = (m_iHighlighted < 0) ? nullptr : m_Buttons.FindIndex(m_iHighlighted);
	CMFCToolBarButton* pSelButton =
		(posSel == nullptr) ? nullptr :(CMFCToolBarButton*) m_Buttons.GetAt(posSel);

	switch (nChar)
	{
	case VK_RETURN:
		if (pSelButton != nullptr)
		{
			GetOwner()->SendMessageW(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
			OnSendCommand(pSelButton);
			return TRUE;
		}
		break;
	}

	return CMFCPopupMenuBar::OnKey(nChar);
}

void PropGridColorBar::OnMouseLeave()
{
	if (mPropGridCtrl == nullptr)
	{
		return CMFCToolBar::OnMouseLeave();
	}

	if (m_hookMouseHelp != nullptr ||
		(m_bMenuMode && !IsCustomizeMode() && GetDroppedDownMenu() != nullptr))
	{
		return;
	}

	m_bTracked = FALSE;
	m_ptLastMouse = CPoint(-1, -1);

	if (m_iHighlighted >= 0)
	{
		int iButton = m_iHighlighted;
		m_iHighlighted = -1;

		OnChangeHot(m_iHighlighted);

		InvalidateButton(iButton);
		UpdateWindow(); // immediate feedback

		GetOwner()->SendMessageW(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	}
}

BOOL __stdcall PropGridColorBar::CreatePalette(
	const CArray<COLORREF, COLORREF>& arColors, CPalette& palette)
{
	if (palette.GetSafeHandle() != nullptr)
	{
		::DeleteObject(palette.Detach());
		ENSURE(palette.GetSafeHandle() == nullptr);
	}

	if (GetGlobalData()->m_nBitsPerPixel != 8)
	{
		return FALSE;
	}

	static const int PGCB_MAX_COLOURS = 100;
	int nNumColours = (int) arColors.GetSize();
	if (nNumColours == 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	ASSERT(nNumColours <= PGCB_MAX_COLOURS);
	if (nNumColours > PGCB_MAX_COLOURS)
	{
		nNumColours = PGCB_MAX_COLOURS;
	}

	// Create the palette
	struct
	{
		LOGPALETTE    LogPalette;
		PALETTEENTRY  PalEntry[PGCB_MAX_COLOURS];
	}
	pal;

	LOGPALETTE* pLogPalette = (LOGPALETTE*) &pal;
	pLogPalette->palVersion    = 0x300;
	pLogPalette->palNumEntries = (WORD) nNumColours;

	for (int i = 0; i < nNumColours; i++)
	{
		pLogPalette->palPalEntry[i].peRed   = GetRValue(arColors[i]);
		pLogPalette->palPalEntry[i].peGreen = GetGValue(arColors[i]);
		pLogPalette->palPalEntry[i].peBlue  = GetBValue(arColors[i]);
		pLogPalette->palPalEntry[i].peFlags = 0;
	}

	palette.CreatePalette(pLogPalette);
	return TRUE;
}

CPalette* PropGridColorBar::SelectPalette(CDC* pDC)
{
	ASSERT_VALID(pDC);

	if (GetGlobalData()->m_nBitsPerPixel != 8)
	{
		if (mPalette.GetSafeHandle() != nullptr)
		{
			DeleteObject(mPalette.Detach());
		}

		return nullptr;
	}

	CPalette* pOldPalette = nullptr;
	if (mPalette.GetSafeHandle() == nullptr)
	{
		// Palette not created yet; create it now
		CreatePalette(mColors, mPalette);
	}
	pOldPalette = pDC->SelectPalette(&mPalette, FALSE);

	ENSURE(pOldPalette != nullptr);
	pDC->RealizePalette();

	return pOldPalette;
}

void PropGridColorBar::SetVertMargin(int nVertMargin)
{
	ASSERT_VALID(this);

	mVertMargin = nVertMargin;
	AdjustLayout();
}

void PropGridColorBar::SetHorzMargin(int nHorzMargin)
{
	ASSERT_VALID(this);

	mHorzMargin = nHorzMargin;
	AdjustLayout();
}

COLORREF PropGridColorBar::GetHighlightedColor() const
{
	ASSERT_VALID(this);

	if (m_iHot < 0)
	{
		return (COLORREF)-1;
	}

	PropGridColorButton* pColorButton = DYNAMIC_DOWNCAST(PropGridColorButton, GetButton(m_iHot));

	if (pColorButton == nullptr)
	{
		return (COLORREF)-1;
	}

	return pColorButton->mColor;
}

} // namespace hsui
