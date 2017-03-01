#pragma once

#include <hsuiHorzScrollBar.h>
#include <hsuiVertScrollBar.h>
#include "hsmnLevelView.h"

namespace hsmn {

class CoordinateGadget : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	CoordinateGadget();
	virtual ~CoordinateGadget();

	void SetPointerInfo(int worldX, int worldY);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;

	int mWorldX;
	int mWorldY;
};

class LevelPanel : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	LevelPanel(xsc::Level* level, CWnd* parent);
	virtual ~LevelPanel();

	virtual BOOL OnCmdMsg(UINT, int, void*, AFX_CMDHANDLERINFO*) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnSetFocus(CWnd* oldWnd);

	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	
protected:
	xsc::Level* mLevel;
	std::unique_ptr<LevelView> mView;

	hsui::HorzScrollBar mHorzScrollBar;
	hsui::VertScrollBar mVertScrollBar;

	CoordinateGadget mCoordinateGadget;

private:
	friend LevelView;
	virtual void SetHorzRange(int start, int end);
	virtual void SetVertRange(int start, int end);
	virtual int GetHorzOffset() const;
	virtual int GetVertOffset() const;
	virtual void UpdatePointerInfo(int xWorld, int yWorld);
};

} // namespace hsmn
