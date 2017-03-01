#pragma once

#include <xscLink.h>
#include <hsprJointPresenter.h>
#include <hsprSegmentPresenter.h>

namespace hspr {

class LinkPresenter;
typedef xsc::Pointer<LinkPresenter> LinkPresenterPtr;
typedef std::list<LinkPresenter*> LinkPresenterList;
typedef std::vector<LinkPresenter*> LinkPresenterVec;
typedef std::unordered_set<LinkPresenter*> LinkPresenterSet;
typedef std::unordered_map<xsc::Component*, LinkPresenter*> ComponentToLinkPresenter;

class HSPR_API LinkPresenter : public Presenter
{
public:
	static void DrawCandidate(ID2D1DeviceContext*, xsc::MPVector, xsc::MPVector, bool);

public:
	LinkPresenter(xsc::Link* link, ILevelView* levelView);
	virtual ~LinkPresenter();

	virtual PresenterType GetPresenterType() const override;

	virtual void OnChildAddition(const xsc::ComponentSet& components) override;
	virtual void OnChildRemoval(const xsc::ComponentSet& components) override;

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;
	virtual void CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees) override;

	virtual void BuildSnapTargets(PresenterVec&, PresenterVec&) override;
	
	virtual void Render(ID2D1DeviceContext* context, double t) override;
	virtual void DrawMoveGhost(ID2D1DeviceContext* context) override;
	
protected:
	ComponentToJointPresenter mJoints;
	ComponentToSegmentPresenter mSegments;

	double mFlowPosition;
	double mLastTime;
	xsc::LinkJoint* mSourceJoint;

	// ================================================================================================================
	private: friend Module;
	static void InitLinkPresenter();
	static void TermLinkPresenter();
	// ================================================================================================================
};

} // namespace hspr
