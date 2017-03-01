#pragma once

#include <xscBlock.h>
#include <hsprPresenter.h>
#include <hsprIVisual.h>

namespace hspr {

class CornerPresenter;
typedef xsc::Pointer<CornerPresenter> CornerPresenterPtr;
typedef std::list<CornerPresenter*> CornerPresenterList;
typedef std::vector<CornerPresenter*> CornerPresenterVec;
typedef std::unordered_set<CornerPresenter*> CornerPresenterSet;
typedef std::unordered_map<xsc::Component*, CornerPresenter*> ComponentToCornerPresenter;

class HSPR_API CornerPresenter
	:
	public Presenter,
	public IVisual
{
public:
	CornerPresenter(xsc::Block* block, Presenter* parent, ILevelView* levelView, xsc::MPRectangle::Corner);
	virtual ~CornerPresenter();

	virtual PresenterType GetPresenterType() const override;
	virtual IVisual* GetVisualInterface() const override;

	xsc::MPRectangle::Corner GetCorner() const;

	virtual void SetHot(bool hot) override;
	virtual bool GetHot() const override;

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;

	virtual void Render(ID2D1DeviceContext* context, double t) override;

protected:
	xsc::MPRectangle::Corner mCorner;
	bool mHot;
};

} // namespace hspr
