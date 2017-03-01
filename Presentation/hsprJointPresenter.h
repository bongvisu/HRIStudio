#pragma once

#include <xscLinkJoint.h>
#include <hsprPresenter.h>
#include <hsprIPointSnapTarget.h>

namespace hspr {

class JointPresenter;
typedef xsc::Pointer<JointPresenter> JointPresenterPtr;
typedef std::list<JointPresenter*> JointPresenterList;
typedef std::vector<JointPresenter*> JointPresenterVec;
typedef std::unordered_set<JointPresenter*> JointPresenterSet;
typedef std::unordered_map<xsc::Component*, JointPresenter*> ComponentToJointPresenter;

class HSPR_API JointPresenter
	:
	public Presenter,
	public IVisual,
	public ISelectable,
	public IPointSnapTarget
{
public:
	static const int FIXED_SIZE;

public:
	JointPresenter(xsc::LinkJoint* joint, Presenter* parent, ILevelView* levelView);
	virtual ~JointPresenter();

	virtual PresenterType GetPresenterType() const override;
	virtual IVisual* GetVisualInterface() const override;
	virtual ISelectable* GetSelectableInterface() const override;

	xsc::MPVector GetLocation() const;

	virtual void SetHot(bool hot) override;
	virtual bool GetHot() const override;

	virtual void SetSelected(bool selected) override;
	virtual bool GetSelected() const override;

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;
	virtual void CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees) override;

	virtual void BuildSnapTargets(PresenterVec&, PresenterVec&) override;
	virtual SnapResult FindSnap(const xsc::MPVector&, const PresenterVec&, const PresenterVec&) const override;

	virtual const xsc::MPVector& GetSnapPoint() const override;
	virtual const xsc::MPRectangle& GetSnapBound() const override;

	virtual void Render(ID2D1DeviceContext* context, double t) override;
	virtual void DrawMoveGhost(ID2D1DeviceContext* context) override;
	virtual void DrawSnapEffect(ID2D1DeviceContext* context) override;

protected:
	bool mHot;
	bool mSelected;

protected:
	xsc::MPVector mSnapPoint;
	xsc::MPRectangle mSnapBound;

protected:
	xsc::ConnectableVec mTempAnchors;
};

} // namespace hspr
