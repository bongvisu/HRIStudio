#pragma once

#include <xscNotification.h>
#include <xscComponent.h>
#include <hsmoModel.h>
#include <hsuiDockablePane.h>
#include <hsuiTreeCtrl.h>

namespace hsmn {

class ProjectPane
	:
	public hsui::DockablePane,
	public xsc::INotiListener,
	public hsui::TreeCtrl::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	ProjectPane();
	virtual ~ProjectPane();

	virtual void OnNotiChildAddition(xsc::Component*, const xsc::ComponentSet&) override;	
	virtual void OnNotiChildRemoval(xsc::Component*, const xsc::ComponentSet&) override;
	virtual void OnNotiPropertyChange(xsc::Component*, xsc::Entity::PropID) override;

	virtual void OnSelectionChanged(hsui::TreeCtrl*) override;									
	virtual void OnItemDoubleClicked(hsui::TreeCtrl*) override;
	virtual void OnLButtonDownOnItem(hsui::TreeCtrl* treeCtrl) override;

protected:
	void AddComponent(xsc::Component* component);

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnPaint();
	
	afx_msg void OnContextMenu(CWnd* wnd, CPoint point);
	afx_msg void OnSetAsActiveModel();
	afx_msg void OnCloseModel();

protected:
	hsui::TreeCtrl mView;

	typedef std::unordered_map<xsc::Component*, int> ComponentToHandle;
	ComponentToHandle mComponentToHandle;
	hsmo::Model* mActiveModel;

	xsc::ComponentVec mTemporaries;
};

} // namespace hsmn
