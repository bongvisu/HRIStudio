#pragma once

#include <hsuiPropGridColorBar.h>

namespace hsui {

class PropGridCtrl;

class PropGridColorPopupMenu : public CMFCPopupMenu
{
	DECLARE_DYNAMIC(PropGridColorPopupMenu)
	DECLARE_MESSAGE_MAP()
	
	friend class PropGridColorProperty;

public:
	PropGridColorPopupMenu(
		const CArray<COLORREF, COLORREF>& colors,
		COLORREF color,
		LPCTSTR lpszAutoColor,
		LPCTSTR lpszOtherColor,
		LPCTSTR lpszDocColors,
		CList<COLORREF,COLORREF>& lstDocColors,
		int nColumns,
		int nHorzDockRows,
		int nVertDockColumns,
		COLORREF colorAutomatic,
		UINT uiCommandID,
		BOOL bStdColorDlg = FALSE)
		:
		mColorBar(
			colors,
			color,
			lpszAutoColor,
			lpszOtherColor,
			lpszDocColors,
			lstDocColors,
			nColumns,
			nHorzDockRows,
			nVertDockColumns,
			colorAutomatic,
			uiCommandID)
	{
		mColorBar.mStdColorDlg = bStdColorDlg;
	}

	PropGridColorPopupMenu(
		const CArray<COLORREF, COLORREF>& colors,
		COLORREF color,
		LPCTSTR lpszAutoColor,
		LPCTSTR lpszOtherColor,
		LPCTSTR lpszDocColors,
		CList<COLORREF,COLORREF>& lstDocColors,
		int nColumns,
		COLORREF colorAutomatic)
		:
		mColorBar(
			colors,
			color,
			lpszAutoColor,
			lpszOtherColor,
			lpszDocColors,
			lstDocColors,
			nColumns,
			-1,
			-1,
			colorAutomatic,
			(UINT)-1)
	{
	}

	virtual ~PropGridColorPopupMenu();

	void SetPropList(PropGridCtrl* pWndList) { mColorBar.SetPropList(pWndList); }

	virtual CMFCPopupMenuBar* GetMenuBar() { return &mColorBar; }
	virtual CPane* CreateTearOffBar(CFrameWnd* wndMain, UINT uiID, LPCTSTR name);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	PropGridColorBar mColorBar;
	BOOL mEnabledInCustomizeMode;
};

} // namespace hsui
