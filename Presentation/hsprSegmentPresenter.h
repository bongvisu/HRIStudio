#pragma once

#include <xscLinkSegment.h>
#include <hsprPresenter.h>
#include <hsprISegmentSnapTarget.h>

namespace hspr {

class SegmentPresenter;
typedef xsc::Pointer<SegmentPresenter> SegmentPresenterPtr;
typedef std::list<SegmentPresenter*> SegmentPresenterList;
typedef std::vector<SegmentPresenter*> SegmentPresenterVec;
typedef std::unordered_set<SegmentPresenter*> SegmentPresenterSet;
typedef std::unordered_map<xsc::Component*, SegmentPresenter*> ComponentToSegmentPresenter;

class HSPR_API SegmentPresenter
	:
	public Presenter,
	public IVisual,
	public ISelectable,
	public ISegmentSnapTarget
{
public:
	SegmentPresenter(xsc::LinkSegment* segment, Presenter* parent, ILevelView* levelView);
	virtual ~SegmentPresenter();

	virtual PresenterType GetPresenterType() const override;
	virtual IVisual* GetVisualInterface() const override;
	virtual ISelectable* GetSelectableInterface() const override;

	xsc::MPVector GetCrossPoint(const xsc::MPVector& approximate) const;

	virtual void SetHot(bool hot) override;
	virtual bool GetHot() const override;

	virtual void SetSelected(bool selected) override;
	virtual bool GetSelected() const override;

	virtual int GetSegmentLength() const override;
	virtual xsc::MPVector::Axis GetSegmentAlignment() const override;
	virtual const xsc::MPVector& GetSnapPoint0() const override;
	virtual const xsc::MPVector& GetSnapPoint1() const override;
	virtual xsc::MPRectangle GetSnapBound() const override;

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;
	virtual void CollectWith(const xsc::MPRectangle& rect, PresenterSet& selected) override;

	virtual void BuildSnapTargets(PresenterVec&, PresenterVec&) override;
	virtual SnapResult FindSnap(const xsc::MPVector&, const PresenterVec&, const PresenterVec&) const override;

	virtual void Render(ID2D1DeviceContext* context, double t) override;
	virtual void DrawMoveGhost(ID2D1DeviceContext* context) override;
	virtual void DrawSnapEffect(ID2D1DeviceContext* context) override;
	bool DrawFlowMarker(ID2D1DeviceContext* context, double target, double& travelled);

private:
	bool mHot;
	bool mSelected;

protected:
	int mLength;
	xsc::MPVector::Axis mAlignment;

	xsc::MPVector mSnapPoint0;
	xsc::MPVector mSnapPoint1;
	xsc::MPRectangle mSnapBound;

	// ================================================================================================================
	private: friend Module;
	static void InitSegmentPresenter();
	static void TermSegmentPresenter();
	// ================================================================================================================
};

} // namespace hspr
