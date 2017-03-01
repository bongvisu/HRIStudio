#include "hsprPrecompiled.h"
#include "hsprBlockPresenter.h"
#include "hsprResource.h"
#include "hsprILevelView.h"
#include "hsprIPointSnapTarget.h"
#include "hsprISegmentSnapTarget.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

unordered_map<const wchar_t*, BlockPresenter::FactoryFunction> BlockPresenter::msFactory;
unordered_map<const wchar_t*, std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*>> BlockPresenter::msBlockBitmaps;

BlockPresenter* BlockPresenter::CreateDynamicInstance(Block* block, ILevelView* levelView)
{
	const auto iter = msFactory.find(block->GetRtti().GetName());
	assert(iter != msFactory.end());
	return iter->second(block, levelView);
}

ID2D1Bitmap1* BlockPresenter::GetCanonicalImage(const wchar_t* rttiName)
{
	return msBlockBitmaps[rttiName].second;
}

BlockPresenter::BlockPresenter(Block* block, ILevelView* levelView)
	:
	Presenter(block, nullptr, levelView),
	mCorners(MPRectangle::NUMBER_OF_CORNERS),
	mBitmap(nullptr),
	mHot(false),
	mSelected(false),
	mPrimary(false)
{
	for (int i = 0; i < MPRectangle::NUMBER_OF_CORNERS; ++i)
	{
		auto corner = static_cast<MPRectangle::Corner>(i);
		mCorners[i] = new CornerPresenter(block, this, levelView, corner);
		mChildren.insert(mCorners[i]);
	}

	PortVec ports;
	block->GetAllPorts(ports);
	for (auto port : ports)
	{
		mPorts[port] = new PortPresenter(port, this, levelView);
		mChildren.insert(mPorts[port]);
	}

	auto biter = msBlockBitmaps.find(block->GetRtti().GetName());
	assert(biter != msBlockBitmaps.end());
	mBitmap = biter->second.first;

	UpdateBound();

	mComponent->RegisterListener(static_cast<IComponentListener*>(this));
}

BlockPresenter::~BlockPresenter()
{
	mComponent->UnregisterListener(static_cast<IComponentListener*>(this));
}

PresenterType BlockPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_BLOCK;
}

IVisual* BlockPresenter::GetVisualInterface() const
{
	return static_cast<IVisual*>(const_cast<BlockPresenter*>(this));
}

ISelectable* BlockPresenter::GetSelectableInterface() const
{
	return static_cast<ISelectable*>(const_cast<BlockPresenter*>(this));
}

IAlignable* BlockPresenter::GetAlignableInterface() const
{
	return static_cast<IAlignable*>(const_cast<BlockPresenter*>(this));
}

void BlockPresenter::OnChildAddition(const ComponentSet& components)
{
	for (auto component : components)
	{
		assert(component->IsDerivedFrom(XSC_RTTI(Port)));
		mPorts[component] = new PortPresenter(static_cast<Port*>(component), this, mLevelView);
		mChildren.insert(mPorts[component]);
	}
}

void BlockPresenter::OnChildRemoval(const ComponentSet& components)
{
	for (auto component : components)
	{
		assert(component->GetParent() == mComponent);
		assert(component->IsDerivedFrom(XSC_RTTI(Port)));
		
		Port* port = static_cast<Port*>(component);
		ComponentToPortPresenter::const_iterator pit = mPorts.find(component);
		assert(pit != mPorts.end());
		PortPresenter* presenter = pit->second;

		mLevelView->InvalidateInteractionFor(presenter);
			
		mPorts.erase(port);
		mChildren.erase(presenter);
	}
}

void BlockPresenter::SetHot(bool hot)
{
	mHot = hot;
}

bool BlockPresenter::GetHot() const
{
	return mHot;
}

void BlockPresenter::SetSelected(bool selected)
{
	mSelected = selected;
}

bool BlockPresenter::GetSelected() const
{
	return mSelected;
}

void BlockPresenter::SetPrimary(bool primary)
{
	mPrimary = primary;
}

bool BlockPresenter::GetPrimary() const
{
	return mPrimary;
}

void BlockPresenter::UpdateBound()
{
	Block* block = static_cast<Block*>(mComponent);

	mBound = block->GetGeometry();

	for (const auto corner : mCorners)
	{
		corner->UpdateBound();
		mBound.Combine(corner->GetBound());
	}

	for (const auto port : mPorts)
	{
		port.second->UpdateBound();
		mBound.Combine(port.second->GetBound());
	}
}

void BlockPresenter::CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees)
{
	Block* block = static_cast<Block*>(mComponent);
	if (rect.Contains(block->GetGeometry()))
	{
		containees.insert(this);
	}
}

Presenter* BlockPresenter::FindHit(const xsc::MPVector& point) const
{
	if (false == mBound.Contains(point))
	{
		return nullptr;
	}

	Block* block = static_cast<Block*>(mComponent);
	Presenter* hitElement = nullptr;

	if (mSelected)
	{
		for (const auto& corner : mCorners)
		{
			hitElement = corner->FindHit(point);
			if (hitElement) return hitElement;
		}
	}

	for (int i = 0; i < MPRectangle::NUMBER_OF_EDGES; ++i)
	{
		const PortVec& ports = block->GetPorts(static_cast<MPRectangle::Edge>(i));
		for (const auto port : ports)
		{
			ComponentToPortPresenter::const_iterator iter = mPorts.find(port);
			assert(iter != mPorts.end());
			hitElement = iter->second->FindHit(point);
			if (hitElement) return hitElement;
		}
	}

	if (block->GetGeometry().Contains(point))
	{
		hitElement = const_cast<BlockPresenter*>(this);
	}

	return hitElement;
}

void BlockPresenter::BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets)
{
	for (const auto port : mPorts)
	{
		port.second->BuildSnapTargets(pointTargets, segmentTargets);
	}
}

Presenter::SnapResult BlockPresenter::FindSnap(const MPVector& delta,
	const PresenterVec& pointTargets, const PresenterVec& segmentTargets) const
{
	SnapResult snapResult;

	int psDistance = std::numeric_limits<int>::max();
	int segmentLength = std::numeric_limits<int>::min();
	for (const auto port : mPorts)
	{
		MPVector point(port.second->GetSnapPoint() + delta);

		for (const auto segmentTarget : segmentTargets)
		{
			ISegmentSnapTarget* segmentST = dynamic_cast<ISegmentSnapTarget*>(segmentTarget);
			if (segmentST->GetSnapBound().Contains(point))
			{
				if (segmentST->GetSegmentAlignment() == MPVector::AXIS_X)
				{
					int diff = segmentST->GetSnapPoint0().y - point.y;
					int absDiff = std::abs(diff);
					if (absDiff < psDistance)
					{
						psDistance = absDiff;

						snapResult.presenter = segmentTarget;
						snapResult.offset.x = 0;
						snapResult.offset.y = diff;
					}
					else if (absDiff == psDistance)
					{
						if (segmentST->GetSegmentLength() > segmentLength)
						{
							segmentLength = segmentST->GetSegmentLength();

							snapResult.presenter = segmentTarget;
							snapResult.offset.x = 0;
							snapResult.offset.y = diff;
						}
					}
				}
				else
				{
					int diff = segmentST->GetSnapPoint0().x - point.x;
					int absDiff = std::abs(diff);
					if (absDiff < psDistance)
					{
						psDistance = absDiff;

						snapResult.presenter = segmentTarget;
						snapResult.offset.x = diff;
						snapResult.offset.y = 0;
					}
					else if (absDiff == psDistance)
					{
						if (segmentST->GetSegmentLength() > segmentLength)
						{
							segmentLength = segmentST->GetSegmentLength();

							snapResult.presenter = segmentTarget;
							snapResult.offset.x = diff;
							snapResult.offset.y = 0;
						}
					}
				}
			}
		}
	}

	float ppDistance = std::numeric_limits<float>::max();
	for (const auto port : mPorts)
	{
		MPVector point(port.second->GetSnapPoint() + delta);

		for (const auto pointTarget : pointTargets)
		{
			IPointSnapTarget* pointST = dynamic_cast<IPointSnapTarget*>(pointTarget);
			if (pointST->GetSnapBound().Contains(point))
			{
				MPVector diff(pointST->GetSnapPoint() - point);
				float distance = (float)(diff.x * diff.x + diff.y * diff.y);
				if (distance < ppDistance)
				{
					ppDistance = distance;

					snapResult.presenter = pointTarget;
					snapResult.offset = diff;
				}
			}
		}
	}
	
	return snapResult;
}

void BlockPresenter::GetUnoccupiedInputPorts(PresenterVec& ports) const
{
	for (auto portPresenter : mPorts)
	{
		Port* port = static_cast<Port*>(portPresenter.first);
		if (port->GetPortType() == PORT_TYPE_INPUT)
		{
			ports.push_back(portPresenter.second);
		}
	}
}

void BlockPresenter::GetUnoccupiedOutputPorts(PresenterVec& ports) const
{
	for (auto portPresenter : mPorts)
	{
		Port* port = static_cast<Port*>(portPresenter.first);
		if (port->GetPortType() == PORT_TYPE_OUTPUT)
		{
			ports.push_back(portPresenter.second);
		}
	}
}

void BlockPresenter::SetupForResize(MPRectangle::Corner corner)
{
	Block* block = static_cast<Block*>(mComponent);
	mResizeCorner = corner;
	mResizeRect = block->GetGeometry();
}

void BlockPresenter::UpdateResizeRect(const MPVector& delta)
{
	Block* block = static_cast<Block*>(mComponent);
	const MPRectangle& rectOriginal = block->GetGeometry();

	switch (mResizeCorner)
	{
	case MPRectangle::CORNER_LT:
		mResizeRect.l = rectOriginal.l + delta.x;
		mResizeRect.t = rectOriginal.t + delta.y;
		if (mResizeRect.l > mResizeRect.r)
		{
			mResizeRect.l = mResizeRect.r;
		}
		if (mResizeRect.t > mResizeRect.b)
		{
			mResizeRect.t = mResizeRect.b;
		}
		break;

	case MPRectangle::CORNER_LB:
		mResizeRect.l = rectOriginal.l + delta.x;
		mResizeRect.b = rectOriginal.b + delta.y;
		if (mResizeRect.l > mResizeRect.r)
		{
			mResizeRect.l = mResizeRect.r;
		}
		if (mResizeRect.b < mResizeRect.t)
		{
			mResizeRect.b = mResizeRect.t;
		}
		break;

	case MPRectangle::CORNER_RT:
		mResizeRect.r = rectOriginal.r + delta.x;
		mResizeRect.t = rectOriginal.t + delta.y;
		if (mResizeRect.r < mResizeRect.l)
		{
			mResizeRect.r = mResizeRect.l;
		}
		if (mResizeRect.t > mResizeRect.b)
		{
			mResizeRect.t = mResizeRect.b;
		}
		break;

	case MPRectangle::CORNER_RB:
		mResizeRect.r = rectOriginal.r + delta.x;
		mResizeRect.b = rectOriginal.b + delta.y;
		if (mResizeRect.r < mResizeRect.l)
		{
			mResizeRect.r = mResizeRect.l;
		}
		if (mResizeRect.b < mResizeRect.t)
		{
			mResizeRect.b = mResizeRect.t;
		}
		break;
	}
}

const MPRectangle& BlockPresenter::GetResizeGeometry() const
{
	return mResizeRect;
}

void BlockPresenter::DrawResizeGhost(ID2D1DeviceContext* context)
{
	D2D1_RECT_F rect = mResizeRect.ToRectF();
	rect.left += 2;
	rect.top += 2;
	rect.right -= 1;
	rect.bottom -= 1;

	solidBrush->SetColor(NORMAL_COLOR);
	context->DrawRectangle(rect, solidBrush, 1, dashedStrokeStyle);
}

void BlockPresenter::DrawMoveGhost(ID2D1DeviceContext* context)
{
	Block* block = static_cast<Block*>(mComponent);

	D2D1_RECT_F rect(block->GetGeometry().ToRectF());
	rect.left += 2;
	rect.top += 2;
	rect.right -= 1;
	rect.bottom -= 1;

	solidBrush->SetColor(NORMAL_COLOR);
	context->DrawRectangle(rect, solidBrush, 1, dashedStrokeStyle);

	for (const auto& port : mPorts)
	{
		port.second->DrawMoveGhost(context);
	}
}

void BlockPresenter::Render(ID2D1DeviceContext* context, double t)
{
	Block* block = static_cast<Block*>(mComponent);

	const auto& geometry = block->GetGeometry();
	const auto rect(geometry.ToRectF());
	D2D1_RECT_F contentRect;

	solidBrush->SetColor(NORMAL_COLOR);

	// Icon -----------------------------------------------------------------------------------------------------------
	auto imageSize = mBitmap->GetPixelSize();
	int imageWidth = static_cast<int>(imageSize.width);
	int imageHeight = static_cast<int>(imageSize.height);

	contentRect.left = rect.left + (geometry.GetWidth() - imageWidth) / 2;
	contentRect.top = rect.top + (geometry.GetHeight() - imageHeight) / 2;
	contentRect.right = contentRect.left + imageWidth;
	contentRect.bottom = contentRect.top + imageHeight;
	context->DrawBitmap(mBitmap, &contentRect);
	// ----------------------------------------------------------------------------------------------------------------

	// Name -----------------------------------------------------------------------------------------------------------
	contentRect.top = contentRect.bottom - 11;
	contentRect.bottom = contentRect.top + 10;
	contentRect.left -= 150;
	contentRect.right += 150;
	const auto& name = block->GetName();
	context->DrawText(name.c_str(), static_cast<UINT32>(name.length()), textFormatCentered, contentRect, solidBrush);
	// ----------------------------------------------------------------------------------------------------------------

	RenderCommon(context, t, rect);
}

void BlockPresenter::RenderCommon(ID2D1DeviceContext* context, double t, D2D_RECT_F rect)
{
	Block* block = static_cast<Block*>(mComponent);

	// Boundary -------------------------------------------------------------------------------------------------------
	rect.left += 2;
	rect.top += 2;
	rect.right -= 1;
	rect.bottom -= 1;
	if (mHot)
	{
		solidBrush->SetColor(HOT_COLOR);
		context->DrawRectangle(rect, solidBrush, 1);
	}
	else if (mSelected)
	{
		if (mPrimary)
		{
			solidBrush->SetColor(PRIMARY_COLOR);
			context->DrawRectangle(rect, solidBrush, 1);
		}
		else
		{
			solidBrush->SetColor(SELECTED_COLOR);
			context->DrawRectangle(rect, solidBrush, 1);
		}
	}
	// ----------------------------------------------------------------------------------------------------------------

	// Ports ----------------------------------------------------------------------------------------------------------
	for (int i = (MPRectangle::NUMBER_OF_EDGES - 1); i >= 0; --i)
	{
		const PortVec& ports = block->GetPorts(static_cast<MPRectangle::Edge>(i));
		PortVec::const_reverse_iterator iter;
		for (iter = ports.rbegin(); iter != ports.rend(); ++iter)
		{
			ComponentToPortPresenter::const_iterator pit = mPorts.find(*iter);
			assert(pit != mPorts.end());
			pit->second->Render(context, t);
		}
	}
	// ----------------------------------------------------------------------------------------------------------------

	// Corner gadgets -------------------------------------------------------------------------------------------------
	if (mSelected)
	{
		CornerPresenterVec::reverse_iterator iter;
		for (iter = mCorners.rbegin(); iter != mCorners.rend(); ++iter)
		{
			(*iter)->Render(context, t);
		}
	}
	// ----------------------------------------------------------------------------------------------------------------
}

} // namespace hspr
