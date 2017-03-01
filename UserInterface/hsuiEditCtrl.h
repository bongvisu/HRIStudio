#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API EditCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	static HACCEL accelerator;

public:
	EditCtrl();
	virtual ~EditCtrl();

	static const int FIXED_HEIGHT;

	void CreateWnd(CWnd* parent, int x, int y, int w);
	CEdit& GetEditWnd();

	void MoveWnd(int x, int y);
	void ResizeWnd(int w);
	void MoveAndResizeWnd(int x, int y, int w);

	void SetEditText(const CString& text);
	CString GetEditText() const;

	virtual BOOL PreTranslateMessage(MSG* msg) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditKillFocus();
	afx_msg HBRUSH OnCtlColor(CDC* dc, CWnd* wnd, UINT ctlrColor);

protected:
	CEdit mEdit;

	bool mActive;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBorderNormal;
	ID2D1SolidColorBrush* mBorderActive;

	// ================================================================================================================
	private: friend Module;
	static void InitEditCtrl();
	static void TermEditCtrl();
	// ================================================================================================================
};

} // namespace mco
