#pragma once

#include <xscPort.h>
#include <hsprPresenter.h>
#include <hsprIPointSnapTarget.h>

namespace hspr {

class PortPresenter;
typedef xsc::Pointer<PortPresenter> PortPresenterPtr;
typedef std::list<PortPresenter*> PortPresenterList;
typedef std::vector<PortPresenter*> PortPresenterVec;
typedef std::unordered_set<PortPresenter*> PortPresenterSet;
typedef std::unordered_map<xsc::Component*, PortPresenter*> ComponentToPortPresenter;

class HSPR_API PortPresenter
	:
	public Presenter,
	public IVisual,
	public IPointSnapTarget
{
public:
	PortPresenter(xsc::Port* port, Presenter* parent, ILevelView* levelView);
	virtual ~PortPresenter();

	virtual PresenterType GetPresenterType() const override;
	virtual IVisual* GetVisualInterface() const override;

	bool IsOccupied() const;
	bool IsUnoccupied() const;
	xsc::PortType GetPortType() const;
	xsc::MPVector GetConnectionPoint() const;

	virtual void SetHot(bool hot) override;
	virtual bool GetHot() const override;

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;
	
	virtual void BuildSnapTargets(PresenterVec&, PresenterVec&) override;
	virtual const xsc::MPVector& GetSnapPoint() const override;
	virtual const xsc::MPRectangle& GetSnapBound() const override;

	virtual void Render(ID2D1DeviceContext* context, double t) override;
	virtual void DrawMoveGhost(ID2D1DeviceContext* context) override;
	virtual void DrawSnapEffect(ID2D1DeviceContext* context) override;

protected:
	bool mHot;

protected:
	xsc::MPVector mSnapPoint;
	xsc::MPRectangle mSnapBound;

	// ================================================================================================================
	private: friend Module;
	static void InitPortPresenter();
	static void TermPortPresenter();
	// ================================================================================================================
};

} // namespace hspr
