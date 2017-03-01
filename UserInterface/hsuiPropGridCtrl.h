#pragma once

#include <hsuiPropGridProperty.h>
#include <hsuiPropGridColorProperty.h>
#include <hsuiPropGridFileProperty.h>
#include <hsuiPropGridFontProperty.h>

namespace hsui {

class HSUI_API PropGridCtrl : public CWnd
{
	DECLARE_DYNAMIC(PropGridCtrl)
	DECLARE_MESSAGE_MAP()

	friend class PropGridProperty;
	friend class PropGridColorProperty;
	friend class PropGridColorBar;

public:
	class IMediator
	{
	public:
		virtual void ToModel(PropGridProperty*) = 0;
		virtual void FromModel(PropGridProperty*) = 0;

	protected:
		virtual ~IMediator();
	};

public:
	static const UINT WND_ID_INPLACE = 3;

	PropGridCtrl();
	virtual ~PropGridCtrl();

	ID2D1Bitmap* comboDropdownButtonImageN;
	ID2D1Bitmap* comboDropdownButtonImageP;

public:
	virtual BOOL Create(DWORD style, const RECT& rect, CWnd* parentWnd, UINT id);
	virtual BOOL PreTranslateMessage(MSG* msg) override;
	virtual CScrollBar* GetScrollBarCtrl(int bar/*SB_HORZ|SB_VERT*/) const;

	void EnableHeaderCtrl(BOOL enable = TRUE, LPCTSTR leftColumn = L"Property", LPCTSTR rightColumn = L"Value");
	BOOL IsHeaderCtrl() const { return mEnableHeaderCtrl; }

	void EnableDescriptionArea(BOOL bEnable = TRUE);
	BOOL IsDescriptionArea() const { return mEnableDescriptionArea; }

	int GetDescriptionHeight() const { return mDescrHeight; }
	int GetDescriptionRows() const { return mDescrRows; }
	void SetDescriptionRows(int nDescRows);

	void SetAlphabeticMode(BOOL bSet = TRUE);
	BOOL IsAlphabeticMode() const { return mAlphabeticMode; }

	void SetVSDotNetLook(BOOL bSet = TRUE);
	BOOL IsVSDotNetLook() const { return mVSDotNetLook; }

	void MarkModifiedProperties(BOOL bMark = TRUE, BOOL bRedraw = TRUE);
	BOOL IsMarkModifiedProperties() const { return mMarkModifiedProperties; }

	void ResetOriginalValues(BOOL bRedraw = TRUE);

	void SetBoolLabels(LPCTSTR trueString, LPCTSTR falseString);
	void SetListDelimiter(TCHAR c);

	CRect GetListRect() const { return mRectList; }

	int GetPropertyColumnWidth() const { return mLeftColumnWidth; }
	int GetHeaderHeight() const { return mHeaderHeight; }
	int GetRowHeight() const { return mRowHeight; }

	virtual CMFCHeaderCtrl& GetHeaderCtrl() { return mHeader; }

	int GetLeftColumnWidth() const { return mLeftColumnWidth; }
	BOOL IsGroupNameFullWidth() const { return mGroupNameFullWidth; }

	void SetGroupNameFullWidth(BOOL groupNameFullWidth = TRUE, BOOL redraw = TRUE);

	BOOL IsShowDragContext() const { return mShowDragContext; }
	void SetShowDragContext(BOOL bShowDragContext = TRUE)
	{
		mShowDragContext = bShowDragContext;
	}

	// Use (COLORREF)-1 for the default color
	void SetCustomColors(
		COLORREF clrBackground,
		COLORREF clrText,
		COLORREF clrGroupBackground,
		COLORREF clrGroupText,
		COLORREF clrDescriptionBackground,
		COLORREF clrDescriptionText,
		COLORREF clrLine);

	void GetCustomColors(
		COLORREF& clrBackground,
		COLORREF& clrText,
		COLORREF& clrGroupBackground,
		COLORREF& clrGroupText,
		COLORREF& clrDescriptionBackground,
		COLORREF& clrDescriptionText,
		COLORREF& clrLine);

	COLORREF GetTextColor() const
	{
		return mTextColor == (COLORREF)-1 ? GetGlobalData()->clrWindowText : mTextColor;
	}

	COLORREF GetBkColor() const
	{
		return mBackgroundColor == (COLORREF)-1 ? GetGlobalData()->clrWindow : mBackgroundColor;
	}

	CFont& GetBoldFont() { return mFontBold; }

	BOOL IsAlwaysShowUserToolTip() const { return mAlwaysShowUserTT; }
	void AlwaysShowUserToolTip(BOOL bShow = TRUE) { mAlwaysShowUserTT = bShow; }

	BOOL DrawControlBarColors() const { return mUseControlBarColors; }

public:
	int AddProperty(PropGridProperty* prop, BOOL redraw = TRUE, BOOL adjustLayout = TRUE);
	BOOL DeleteProperty(PropGridProperty*& prop, BOOL redraw = TRUE, BOOL adjustLayout = TRUE);
	void RemoveAll();

	PropGridProperty* GetProperty(int nIndex) const;
	int GetPropertyCount() const { return (int)mProps.GetCount(); }

	PropGridProperty* FindItemByData(DWORD_PTR data, BOOL searchSubItems = TRUE) const;

	PropGridProperty* HitTest(CPoint pt, PropGridProperty::ClickArea* area = nullptr, BOOL propsOnly = FALSE) const;

	void SetCurSel(PropGridProperty* prop, BOOL redraw = TRUE);
	PropGridProperty* GetCurSel() const { return mSelection; }

	void EnsureVisible(PropGridProperty* prop, BOOL bExpandParents = FALSE);
	void ExpandAll(BOOL expand = TRUE);

	void CloseColorPopup();
	void UpdateColor(COLORREF color);
	void AdjustLayout();

	void OnPropertyChanged(PropGridProperty* prop);

public:
	virtual void OnChangeSelection(PropGridProperty*, PropGridProperty*) { }

	virtual BOOL EditItem(PropGridProperty* prop, LPPOINT clickPoint = nullptr);
	virtual void OnClickButton(CPoint point);
	virtual BOOL EndEditItem(BOOL updateData = TRUE);
	virtual BOOL ValidateItemData(PropGridProperty*) { return TRUE; }

	virtual int OnDrawProperty(CDC* dc, PropGridProperty* prop) const;
	virtual void InitHeader();

protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* result);

protected:
	virtual void Init();

	virtual void OnFillBackground(CDC* dc, CRect rectClient);

	virtual void OnDraw(CDC* dc);
	virtual void OnDrawBorder(CDC* dc);
	virtual void OnDrawList(CDC* dc);
	virtual void OnDrawDescription(CDC* dc, CRect rect);

	virtual BOOL ProcessClipboardAccelerators(UINT nChar);
	virtual int CompareProps(const PropGridProperty* prop1, const PropGridProperty* prop2) const;
	virtual void NotifyAccessibility(PropGridProperty* prop);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnKillFocus(CWnd* newWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnVScroll(UINT sbCode, UINT pos, CScrollBar* scrollBar);
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* wnd, UINT hitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT repCnt, UINT flags);
	afx_msg void OnChar(UINT nChar, UINT repCnt, UINT flags);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlrColor);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL calcValidRects, NCCALCSIZE_PARAMS FAR* ncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT flags, CPoint point);
	afx_msg void OnSetFont(CFont* pFont, BOOL redraw);
	afx_msg HFONT OnGetFont();
	afx_msg void OnHeaderItemChanged(NMHDR* nmhdr, LRESULT* result);
	afx_msg void OnHeaderTrack(NMHDR* nmhdr, LRESULT* result);
	afx_msg void OnHeaderEndTrack(NMHDR* nmhdr, LRESULT* result);
	afx_msg void OnSpinDeltaPos(NMHDR* nmhdr, LRESULT* result);
	afx_msg LRESULT OnUpdateSpin(WPARAM, LPARAM);
	afx_msg void OnStyleChanged(int styleType, LPSTYLESTRUCT styleStruct);
	afx_msg void OnSelectCombo();
	afx_msg void OnCloseCombo();
	afx_msg void OnEditKillFocus();
	afx_msg void OnComboKillFocus();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* nmhdr, LRESULT* result);
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(CDC* dc, UINT flags);

	//-----------------
	// Internal helpres
	//-----------------
	HFONT SetCurrFont(CDC* dc);
	void TrackHeader(int offset);
	void TrackDescr(int offset);
	void TrackToolTip(CPoint pt);

	void SetScrollSizes();

	int GetTotalItems(BOOL includeHidden = TRUE) const;
	void ReposProperties();

	void CreateBoldFont();
	void CalcEditMargin();

protected:
	CToolTipCtrl mToolTip;							// Tooltip control
	CMFCHeaderCtrl mHeader;							// Property list header control
	CMFCPropertyGridToolTipCtrl mInplaceToolTip;    // Inplace tooltip control
	CScrollBar mVertScrollBar;						// Vertical scroll bar

	BOOL mEnableHeaderCtrl;			// Is header control visible?
	BOOL mEnableDescriptionArea;	// Does description area enabled?
	BOOL mAlwaysShowUserTT;			// Always show user tooltips, even if in-place TT should be activated
	BOOL mAlphabeticMode;			// Use property list in alphabetic (non-"tree") mode
	BOOL mVSDotNetLook;				// Provide MS Visual Studio.NET look (gray groups, flat look)
	BOOL mMarkModifiedProperties;	// Draw modified properties with bold

	CString mTrueString;	// Customized boolean value (e.g. "Yes")
	CString mFalseString;	// Customized boolean value (e.g. "No")

	TCHAR mListDelimeter;	// Customized list delimeter character

	HFONT mFont;		// Property list regular font
	CFont mFontBold;	// Property list bold font
	CRect mRectList;	// Properies area

	int mDescrHeight;			// Description area height
	int mDescrRows;				// Number of rows in description area
	int mTooltipsCount;			// Number of tooltip items
	int mEditLeftMargin;		// Edit control left margin
	int mBoldEditLeftMargin;	// Edit control left margin (bold font)
	int mBorderSize;			// Control border size
	int mHeaderHeight;			// Header control height
	int mRowHeight;				// Height of the single row
	int mLeftColumnWidth;		// Width of the left ("Name") column
	int mVertScrollOffset;		// In rows
	int mVertScrollTotal;
	int mVertScrollPage;

	//-----------------------------------------------------------------------------------
	// Tracking attributes: used for the vertical and description area splitters tracking
	//-----------------------------------------------------------------------------------
	CRect mRectTrackHeader;
	CRect mRectTrackDescr;

	BOOL mTracking;
	BOOL mTrackingDescr;
	BOOL mFocused;				// Control has focus
	BOOL mUseControlBarColors;	// Use colors of the parent control bar
	BOOL mGroupNameFullWidth;	// Display group name in the whole row
	BOOL mShowDragContext;		// Show context while dragging spliters

	CList<PropGridProperty*, PropGridProperty*> mProps;         // List of top-level properties
	CList<PropGridProperty*, PropGridProperty*> mTerminalProps; // List of terminal properties
	PropGridProperty*							mSelection;     // Current selection

	COLORREF mGrayColor; // Special gray color

	//---------------
	// Custom colors
	//---------------
	COLORREF mBackgroundColor;				// Control background color
	COLORREF mTextColor;					// Control foreground color
	COLORREF mGroupBackgroundColor;			// Group background text
	COLORREF mGroupTextColor;				// Group foreground text
	COLORREF mDescriptionBackgroundColor;	// Description background text
	COLORREF mDescriptionTextColor;			// Description foreground text
	COLORREF mLineColor;					// Color of the grid lines

	CBrush mBackgroundBrush;

public:
	//----------------------
	// Accessibility support
	//----------------------
	PropGridProperty* accProp;

	virtual HRESULT get_accChildCount(long* countChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch** dispChild);
	virtual HRESULT get_accName(VARIANT varChild, BSTR* name);
	virtual HRESULT get_accValue(VARIANT varChild, BSTR* value);
	virtual HRESULT get_accDescription(VARIANT varChild, BSTR* description);
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT* varRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT* varState);
	virtual HRESULT get_accHelp(VARIANT varChild, BSTR* help);
	virtual HRESULT get_accHelpTopic(BSTR* helpFile, VARIANT varChild, long* topicID);
	virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR* keyboardShortcut);
	virtual HRESULT get_accFocus(VARIANT* pvarChild);
	virtual HRESULT get_accSelection(VARIANT* pvarChildren);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR* defaultAction);

	virtual HRESULT accSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT accLocation(long* left, long* top, long* width, long* height, VARIANT varChild);
	virtual HRESULT accHitTest(long left, long top, VARIANT* varChild);
};

} // namespace hsui
