#pragma once

namespace hsui {

class PropGridProperty : public CObject
{
	DECLARE_DYNAMIC(PropGridProperty)
	friend class PropGridCtrl;

public:
	// Group
	PropGridProperty(const CString& groupName,
		DWORD_PTR data = 0,
		BOOL isValueList = FALSE);

	// Leaf
	PropGridProperty(const CString& name, const COleVariant& value,
		LPCTSTR descr = nullptr,
		DWORD_PTR data = 0,
		LPCTSTR editMask = nullptr,
		LPCTSTR editTemplate = nullptr,
		LPCTSTR validChars = nullptr);

	virtual ~PropGridProperty();

	enum ClickArea
	{
		ClickExpandBox,
		ClickName,
		ClickValue,
		ClickDescription
	};

	int GetExpandedSubItems(BOOL includeHidden = TRUE) const;
	BOOL AddSubItem(PropGridProperty* prop);
	BOOL RemoveSubItem(PropGridProperty*& prop, BOOL deleteProp = TRUE);

	BOOL AddOption(LPCTSTR option, BOOL insertUnique = TRUE);
	void RemoveAllOptions();

	int GetOptionCount() const;
	LPCTSTR GetOption(int nIndex) const;

	PropGridProperty* HitTest(CPoint point, PropGridProperty::ClickArea* clickArea = nullptr);

	void Expand(BOOL expand = TRUE);
	void Redraw();

	void EnableSpinControl(BOOL enable = TRUE, int minValue = 0, int maxValue = 0);

	virtual void ResetOriginalValue();

	void Show(BOOL show = TRUE, BOOL adjustLayout = TRUE);

protected:
	void Init();
	void SetFlags();
	void SetOwnerList(PropGridCtrl* wndList);
	void Reposition(int& y);
	void AddTerminalProp(CList<PropGridProperty*, PropGridProperty*>& props);

	BOOL IsSubItem(PropGridProperty* prop) const;
	PropGridProperty* FindSubItemByData(DWORD_PTR data) const;

	void ExpandDeep(BOOL expand = TRUE);
	void SetModifiedFlag();

public:
	virtual void OnDrawName(CDC* dc, CRect rect);
	virtual void OnDrawValue(CDC* dc, CRect rect);
	virtual void OnDrawExpandBox(CDC* dc, CRect rectExpand);
	virtual void OnDrawButton(CDC* dc, CRect rectButton);
	virtual void OnDrawDescription(CDC* dc, CRect rect);

	virtual CString FormatProperty();

	virtual BOOL OnUpdateValue();
	virtual BOOL OnEdit(LPPOINT clickPoint);
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& defaultFormat);
	virtual CSpinButtonCtrl* CreateSpinControl(CRect rectSpin);

	virtual BOOL OnEndEdit();

	virtual void OnClickButton(CPoint point);
	virtual BOOL OnClickValue(UINT msg, CPoint point);
	virtual BOOL OnDblClk(CPoint point);

	virtual void OnSelectCombo();
	virtual void OnCloseCombo();

	virtual BOOL OnSetCursor() const;
	virtual BOOL PushChar(UINT nChar);

	virtual CString GetNameTooltip();
	virtual CString GetValueTooltip();

	virtual void OnClickName(CPoint /*point*/) { }
	virtual void OnRClickName(CPoint /*point*/) { }
	virtual void OnRClickValue(CPoint /*point*/, BOOL /*bSelChanged*/) { }

	virtual void OnPosSizeChanged(CRect /*rectOld*/) { }

	virtual void OnSetSelection(PropGridProperty* /*pOldSel*/) { }
	virtual void OnKillSelection(PropGridProperty* /*pNewSel*/) { }

	virtual void AdjustButtonRect();
	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);

protected:
	virtual HBRUSH OnCtlColor(CDC* dc, UINT ctlrColor);
	virtual CComboBox* CreateCombo(CWnd* parent, CRect rect);
	virtual void OnDestroyWindow();

	virtual BOOL OnKillFocus(CWnd* /*pNewWnd*/) { return TRUE; }
	virtual BOOL OnEditKillFocus() { return TRUE; }

	virtual BOOL HasButton() const;

	virtual BOOL IsProcessFirstClick() const { return TRUE; }
	virtual BOOL HasValueField() const { return TRUE; }

	virtual BOOL TextToVar(const CString& text);
	virtual BOOL IsValueChanged() const;

	virtual BOOL OnActivateByTab();
	virtual BOOL OnRotateListValue(BOOL forward);

public:
	LPCTSTR GetName() const { return mName; }
	void SetName(LPCTSTR name, BOOL redraw = TRUE);

	virtual const COleVariant& GetValue() const { return mValue; }
	virtual void SetValue(const COleVariant& varValue);

	const COleVariant& GetOriginalValue() const { return mOriginalValue; }
	void SetOriginalValue(const COleVariant& varValue);

	const CString& GetDescription() const { return mDescr; }
	void SetDescription(const CString& strDescr) { mDescr = strDescr; }

	DWORD_PTR GetData() const { return mData; }
	void SetData(DWORD_PTR dwData) { mData = dwData; }

	BOOL IsGroup() const { return mIsGroup; }
	BOOL IsExpanded() const { return mExpanded; }

	BOOL IsParentExpanded() const;

	virtual BOOL IsSelected() const;
	int GetHierarchyLevel() const;

	void Enable(BOOL enable = TRUE);
	BOOL IsEnabled() const { return mEnabled; }

	void AllowEdit(BOOL allow = TRUE)
	{
		ASSERT(mValue.vt != VT_BOOL);
		mAllowEdit = allow;
	}
	BOOL IsAllowEdit() const { return mAllowEdit; }

	CRect GetRect() const { return mRect; }

	int GetSubItemsCount() const { return (int) mSubItems.GetCount(); }

	PropGridProperty* GetSubItem(int index) const;

	PropGridProperty* GetParent() const { return mParent; }
	BOOL IsInPlaceEditing() const { return mInPlaceEditing; }
	BOOL IsModified() const { return mIsModified; }
	BOOL IsVisible() const { return mIsVisible; }

protected:
	COleVariant mValue;			// Property value
	COleVariant mOriginalValue; // Property original value

	DWORD_PTR mData;			// User-defined data
	DWORD     mFlags;			// Property flags

	CRect mRect;				// Property rectangle (in the prop.list coordinates)
	CRect mRectButton;			// Drop down/open button rectangle

	BOOL mButtonIsDown;			// Is button pressed?
	BOOL mButtonIsFocused;		// Is button focused?
	BOOL mInPlaceEditing;		// Is in InPalce editing mode
	BOOL mIsGroup;				// Is property group?
	BOOL mExpanded;				// Is property expanded (for groups only)
	BOOL mEnabled;				// Is propperty enabled?
	BOOL mAllowEdit;			// Is property editable?
	BOOL mIsValueList;			// This is a value list group?
	BOOL mNameIsTruncated;
	BOOL mValueIsTruncated;
	BOOL mIsModified;			// Is property modified
	BOOL mIsVisible;			// Is property visible

	int mMinValue;
	int mMaxValue;

	CString mName;				// Property name
	CString mDescr;				// Property description
	CString mEditMask;			// Property edit mask (see CMFCMaskedEdit for description)
	CString mEditTemplate;		// Property edit template (see CMFCMaskedEdit for description)
	CString mValidChars;		// Property edit valid chars (see CMFCMaskedEdit for description)
	CStringList mOptions;		// List of combobox items

	CWnd*					mWndInPlace; // Pointer to InPlace editing window
	CComboBox*				mWndCombo;   // Pointer to combbox
	CSpinButtonCtrl*		mWndSpin;    // Pointer to spin button
	PropGridCtrl*			mWndList;    // Pointer to the PropertyList window
	PropGridProperty*		mParent;     // Parent property (NULL for top-level properties)

	CList<PropGridProperty*, PropGridProperty*> mSubItems; // Sub-properies list

public:
	static const DWORD PROP_HAS_LIST;
	static const DWORD PROP_HAS_BUTTON;
	static const DWORD PROP_HAS_SPIN;
};

} // namespace hsui
