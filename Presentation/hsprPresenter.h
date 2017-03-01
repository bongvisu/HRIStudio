#pragma once

#include <xscComponent.h>
#include <hsprILevelView.h>
#include <hsprIVisual.h>
#include <hsprISelectable.h>
#include <hsprIAlignable.h>

namespace hspr {

class Presenter;
typedef xsc::Pointer<Presenter> PresenterPtr;
typedef std::list<Presenter*> PresenterList;
typedef std::vector<Presenter*> PresenterVec;
typedef std::unordered_set<Presenter*> PresenterSet;
typedef std::unordered_map<xsc::Component*, PresenterPtr> ComponentToPresenter;

enum PresenterType
{
	PRESENTER_TYPE_BLOCK,
	PRESENTER_TYPE_PORT,
	PRESENTER_TYPE_CORNER,
	PRESENTER_TYPE_LINK,
	PRESENTER_TYPE_JOINT,
	PRESENTER_TYPE_SEGMENT
};

class HSPR_API Presenter
	:
	public xsc::Object,
	public xsc::IComponentListener
{
public:
	static const int POINTER_ACTION_LATENCY;
	
	static IDWriteTextFormat* textFormatTitle;
	static IDWriteTextFormat* textFormatLeading;
	static IDWriteTextFormat* textFormatCentered;
	static IDWriteTextFormat* textFormatTrailing;

	static ID2D1SolidColorBrush* solidBrush;
	static ID2D1StrokeStyle1* dashedStrokeStyle;
	static ID2D1StrokeStyle1* dashDottedStrokeStyle;

	static void DrawSelectionRect(ID2D1DeviceContext* context, const xsc::MPRectangle& rect);

public:
	virtual ~Presenter();
	
	virtual PresenterType GetPresenterType() const = 0;
	virtual IVisual* GetVisualInterface() const;
	virtual ISelectable* GetSelectableInterface() const;
	virtual IAlignable* GetAlignableInterface() const;

	virtual void OnChildAddition(const xsc::ComponentSet& components) override;
	virtual void OnChildRemoval(const xsc::ComponentSet& components) override;
	virtual void OnPropertyChange(xsc::Entity::PropID) override;
	virtual void OnNodalGeometryChange() override final;

	xsc::Component* GetComponent() const;
	Presenter* GetParent() const;
	bool IsChild(Presenter* presenter) const;
	bool Contains(Presenter* presenter) const;
	void GetChildren(PresenterVec& children) const;
	void GetChildren(PresenterSet& children) const;

	virtual void UpdateBound();
	const xsc::MPRectangle& GetBound() const;

	virtual Presenter* FindHit(const xsc::MPVector& point) const;
	virtual void CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees);
	
	virtual void Render(ID2D1DeviceContext* context, double t);
	virtual void DrawMoveGhost(ID2D1DeviceContext* context);
	virtual void DrawSnapEffect(ID2D1DeviceContext* context);

	struct HSPR_API SnapResult { SnapResult(); void Clear(); Presenter* presenter; xsc::MPVector offset; };
	virtual SnapResult FindSnap(const xsc::MPVector&, const PresenterVec&, const PresenterVec&) const;
	virtual void BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets);

protected:
	Presenter(xsc::Component* component, Presenter* parent, ILevelView* levelView);

protected:
	xsc::Component* mComponent;
	
	Presenter* mParent;
	std::unordered_set<PresenterPtr, std::hash<Presenter*>> mChildren;

	ILevelView* mLevelView;
	xsc::MPRectangle mBound;

protected:
	static const D2D1_COLOR_F HOT_COLOR;
	static const D2D1_COLOR_F SELECTED_COLOR;
	static const D2D1_COLOR_F PRIMARY_COLOR;
	static const D2D1_COLOR_F NORMAL_COLOR;
	static const D2D1_COLOR_F TEAL_COLOR;
	static const D2D1_COLOR_F PURPLE_COLOR;

	enum { TEXT_BUF_SIZE = 256 };
	static wchar_t msTextBuf[TEXT_BUF_SIZE];

	// ================================================================================================================
	private: friend Module;
	static void InitPresenter();
	static void TermPresenter();
	// ================================================================================================================
};

} // namespace hspr
