#pragma once

#include <xscComponent.h>
#include <xscD2DRenderer.h>
#include <hsmoCustomCommand.h>
#include <hsprPresenter.h>
#include "hsmnDragHandler.h"
#include "hsmnILoopConsumer.h"
#include "hsmnIBlockDropTarget.h"
#include "hsmnDHAddLink.h"
#include "hsmnDHMoveComponents.h"
#include "hsmnDHResizeBlock.h"
#include "hsmnDHSelectGroup.h"

namespace hsmn {

class LevelPanel;

class LevelView
	:
	public CWnd,
	public xsc::IComponentListener,
	public hspr::ILevelView,
	public ILoopConsumer,
	public IBlockDropTarget
{
	DECLARE_MESSAGE_MAP()

public:
	LevelView(xsc::Level* level, CWnd* parent);
	virtual ~LevelView();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void OnChildAddition(const xsc::ComponentSet& components) override;
	virtual void OnChildRemoval(const xsc::ComponentSet& components) override;
	virtual void OnPropertyChange(xsc::Entity::PropID) override;
	virtual void OnNodalGeometryChange() override;

	virtual void InvalidateInteractionFor(hspr::Presenter* const presenter) override;

	virtual void DoLoop(double t) override;
	virtual void OnBlockDrop(const wchar_t* rttiName, CPoint point) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();

	afx_msg void OnSize(UINT type, int cx, int cy);

	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();

	afx_msg void OnRButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* capturingWnd);

	afx_msg void OnKeyDown(UINT key, UINT repeatCount, UINT flags);
	afx_msg void OnKeyUp(UINT, UINT, UINT);
	afx_msg void OnKillFocus(CWnd* newFocus);

	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditSelectAll();

	afx_msg void OnFormatAlignLeft();
	afx_msg void OnFormatAlignRight();
	afx_msg void OnFormatAlignTop();
	afx_msg void OnFormatAlignBottom();
	void AlignBlocks(xsc::MPVector::Direction direction);

	afx_msg void OnFormatMakeSameSize();
	afx_msg void OnFormatMakeSameWidth();
	afx_msg void OnFormatMakeSameHeight();
	void EqualizeBlocks(xsc::MPVector::Axis dim);

	afx_msg void OnFormatRotateBlock();

	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	void Render(double t);

	xsc::MPVector ViewToWorld(const xsc::MPVector& point) const;
	xsc::MPVector WorldToView(const xsc::MPVector& point) const;
	xsc::MPRectangle ViewToWorld(const xsc::MPRectangle& rect) const;
	xsc::MPRectangle WorldToView(const xsc::MPRectangle& rect) const;
	
	double GetAppTime() const;
	
	void InitPresenters();
	
	void UpdateFrameTitle();
	void UpdateBound();
	void UpdateScrollParameters();

	void UpdateHotPresenter(const CPoint& viewPoint);

	void ResetPrimary(hspr::Presenter* newPrimary);
	void InsertSelection(hspr::Presenter* presenter);
	void RemoveSelection(hspr::Presenter* presenter);
	void ClearSelection();
	void ReportActiveSelections();

protected:
	// ----------------------------------------------------------------------------------------------------------------
	xsc::Level* mLevel;

	hspr::ComponentToPresenter mPresenters;
	xsc::MPRectangle mBound;

	hspr::Presenter* mHotPresenter;

	bool mActiveAddition;
	hspr::Presenter* mPrimaryBlock;
	hspr::PresenterSet mSelectedPresenters;
	// ----------------------------------------------------------------------------------------------------------------

	LevelPanel* mPanel;
	xsc::MPVector mViewSize;
	CPoint mCursorPos;

	xsc::D2DRenderer mRenderer;
	
	// Dragging -------------------------------------------------------------------------------------------------------
	std::unique_ptr<DragHandler> mDragHandlers[NUMBER_OF_DRAG_HANDLER_TYPES];
	DragHandler* mDragHandler;
	bool mTrackingMouseLeave;
	// ----------------------------------------------------------------------------------------------------------------

	// Keying ---------------------------------------------------------------------------------------------------------
	bool mDeleteKeyDown;
	hsmo::CustomCommandPtr mMoveByKeyCommand;
	xsc::MPVector::Direction mMoveDirection;
	// ----------------------------------------------------------------------------------------------------------------

	hspr::PresenterVec mTempPresenterVec;
	xsc::ComponentSet mTempComponentSet;

	friend DHSelectGroup;
	friend DHMoveComponents;
	friend DHResizeBlock;
	friend DHAddLink;
};

} // namespace hsmn
