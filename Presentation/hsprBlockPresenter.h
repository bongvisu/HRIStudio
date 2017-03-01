#pragma once

#include <hsprPortPresenter.h>
#include <hsprCornerPresenter.h>

namespace hspr {

class BlockPresenter;
typedef xsc::Pointer<BlockPresenter> BlockPresenterPtr;
typedef std::list<BlockPresenter*> BlockPresenterList;
typedef std::vector<BlockPresenter*> BlockPresenterVec;
typedef std::unordered_set<BlockPresenter*> BlockPresenterSet;
typedef std::unordered_map<xsc::Component*, BlockPresenter*> ComponentToBlockPresenter;

class HSPR_API BlockPresenter
	:
	public Presenter,
	public IVisual,
	public ISelectable,
	public IAlignable
{
public:
	typedef BlockPresenter* (* FactoryFunction) (xsc::Block* block, ILevelView* levelView);
	static BlockPresenter* CreateDynamicInstance(xsc::Block* block, ILevelView* levelView);

	static ID2D1Bitmap1* GetCanonicalImage(const wchar_t* rttiName);

public:
	BlockPresenter(xsc::Block* block, ILevelView* levelView);
	virtual ~BlockPresenter();

	virtual PresenterType GetPresenterType() const override;
	virtual IVisual* GetVisualInterface() const override;
	virtual ISelectable* GetSelectableInterface() const override;
	virtual IAlignable* GetAlignableInterface() const override;

	virtual void OnChildAddition(const xsc::ComponentSet& components) override;
	virtual void OnChildRemoval(const xsc::ComponentSet& components) override;
	
	virtual void SetHot(bool hot) override;
	virtual bool GetHot() const override;

	virtual void SetSelected(bool selected) override;
	virtual bool GetSelected() const override;

	virtual void SetPrimary(bool primary) override;
	virtual bool GetPrimary() const override;

	void SetupForResize(xsc::MPRectangle::Corner corner);
	void UpdateResizeRect(const xsc::MPVector& delta);
	const xsc::MPRectangle& GetResizeGeometry() const;
	void DrawResizeGhost(ID2D1DeviceContext* context);

	virtual void UpdateBound() override;
	virtual Presenter* FindHit(const xsc::MPVector& point) const override;
	virtual void CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees) override;

	virtual void BuildSnapTargets(PresenterVec& points, PresenterVec& linears) override;
	virtual SnapResult FindSnap(const xsc::MPVector&, const PresenterVec&, const PresenterVec&) const override;

	void GetUnoccupiedInputPorts(PresenterVec& ports) const;
	void GetUnoccupiedOutputPorts(PresenterVec& ports) const;

	virtual void Render(ID2D1DeviceContext* context, double t) override;
	virtual void DrawMoveGhost(ID2D1DeviceContext* context) override;

protected:
	void RenderCommon(ID2D1DeviceContext* context, double t, D2D_RECT_F rect);

protected:
	CornerPresenterVec mCorners;
	ComponentToPortPresenter mPorts;

	ID2D1Bitmap1* mBitmap;

	bool mHot;
	bool mSelected;
	bool mPrimary;

protected:
	xsc::MPRectangle::Corner mResizeCorner;
	xsc::MPRectangle mResizeRect;

	static std::unordered_map<const wchar_t*, FactoryFunction> msFactory;
	static std::unordered_map<const wchar_t*, std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*>> msBlockBitmaps;
};

} // namespace hspr
