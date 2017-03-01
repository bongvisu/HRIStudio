#pragma once

#include <hsuiPropGridColorButton.h>

namespace hsui {

class PropGridCtrl;

class PropGridColorBar : public CMFCPopupMenuBar
{
	DECLARE_SERIAL(PropGridColorBar)
	DECLARE_MESSAGE_MAP()

	friend class PropGridColorProperty;
	friend class PropGridColorPopupMenu;
	friend class PropGridColorButton;

public:
	PropGridColorBar();
	virtual ~PropGridColorBar();

	virtual BOOL Create(CWnd* parentWnd, DWORD style, UINT id,
		CPalette* palette = nullptr,	// Palette color, NULL - standard 20 colors
		int numColumns = 0,				// Number of columns, -1 - default
		int numRowsDockHorz = 0,		// Number of rows for horz. docking
		int numColDockVert = 0);		// Number of columns for vert. docking

	virtual BOOL CreateControl(CWnd* parentWnd, const CRect& rect,
		UINT id, int numColumns = -1, CPalette* palette = nullptr);

protected:
	PropGridColorBar(const CArray<COLORREF, COLORREF>& colors, COLORREF color,
		LPCTSTR autoColorString, LPCTSTR otherColorString, LPCTSTR docColorString,
		CList<COLORREF,COLORREF>& docColors, int numColumns, int numRowsDockHorz,
		int numColDockVert, COLORREF colorAutomatic, UINT commandID);

	PropGridColorBar(PropGridColorBar& src, UINT commandID);

public:
	void SetDocumentColors(LPCTSTR caption, CList<COLORREF,COLORREF>& docColors, BOOL showWhenDocked = FALSE);
	void ContextToSize(BOOL squareButtons = TRUE, BOOL centerButtons = TRUE);

	void SetColor(COLORREF color);
	COLORREF GetColor() const { return mColorSelected; }

	static void __stdcall SetColorName(COLORREF color, const CString& strName)
	{
		msColorNames.SetAt(color, strName);
	}

	void EnableAutomaticButton(LPCTSTR label, COLORREF colorAutomatic, BOOL enable = TRUE);
	void EnableOtherButton(LPCTSTR label, BOOL altColorDlg = TRUE, BOOL enable = TRUE);

	UINT GetCommandID() const { return mCommandID; }
	void SetCommandID(UINT nCommandID) { mCommandID = nCommandID; }
	BOOL IsTearOff() const { return mIsTearOff; }

	void SetVertMargin(int vertMargin);
	void SetHorzMargin(int horzMargin);

	int GetVertMargin() const { return mVertMargin; }
	int GetHorzMargin()	{ return mHorzMargin; }

	COLORREF GetHighlightedColor() const;

protected:
	virtual BOOL OnSendCommand(const CMFCToolBarButton* button);
	virtual void OnUpdateCmdUI(CFrameWnd* target, BOOL bDisableIfNoHndler);
	virtual void AdjustLocations();
	virtual CSize CalcSize(BOOL vertDock);
	virtual void DoPaint(CDC* dc);
	virtual BOOL OnKey(UINT nChar);

	virtual void Serialize(CArchive& ar);
	virtual void ShowCommandMessageString(UINT cmdID);

	virtual BOOL AllowChangeTextLabels() const{ return FALSE; }
	virtual BOOL AllowShowOnList() const { return TRUE; }

	virtual BOOL OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes);

	virtual void Rebuild();

	virtual BOOL PreTranslateMessage(MSG* msg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* focusWnd);
	afx_msg void OnNcCalcSize(BOOL calcValidRects, NCCALCSIZE_PARAMS FAR* ncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseLeave();

	static int __stdcall InitColors(CPalette* palette, CArray<COLORREF, COLORREF>& colors);
	static BOOL __stdcall CreatePalette(const CArray<COLORREF, COLORREF>& colors, CPalette& palette);

	CSize GetColorGridSize(BOOL vertDock) const;
	int GetExtraHeight(int numColumns) const;
	CPalette* SelectPalette(CDC* dc);

	void SetPropList(PropGridCtrl* wndList) { mPropGridCtrl = wndList; }

protected:
	int mNumColumns;
	int mNumRowsHorz;
	int mNumColumnsVert;
	int mHorzOffset;
	int mVertOffset;
	int mRowHeight;
	int mVertMargin;
	int mHorzMargin;

	BOOL mStdColorDlg;
	BOOL mIsEnabled;
	BOOL mIsTearOff;
	BOOL mShowDocColorsWhenDocked;
	BOOL mInternal; // used in customization

	UINT mCommandID;

	CArray<COLORREF, COLORREF> mColors;
	CList<COLORREF, COLORREF>  mDocColors;

	CSize			mBoxSize;
	CPalette		mPalette;
	COLORREF		mColorSelected; // (-1) - auto
	COLORREF		mColorAutomatic;
	CString			mAutoColorString;
	CString			mOtherColorString;
	CString			mDocColorString;
	PropGridCtrl*	mPropGridCtrl;

	static CMap<COLORREF, COLORREF, CString, LPCTSTR> msColorNames;
};

} // namespace hsui
