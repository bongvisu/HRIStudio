#include "hsprPrecompiled.h"
#include "hsprLinkPresenter.h"
#include "hsprILevelView.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>
#include <xscRangeIteration.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static ID2D1Bitmap1* gsDecoTerminalL;
static ID2D1Bitmap1* gsDecoTerminalU;

void LinkPresenter::InitLinkPresenter()
{
	WicBitmap wicBitmap;
	auto context = D2DRenderer::resourcingContext;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_LINK_TERMINAL_L);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoTerminalL);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_LINK_TERMINAL_U);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoTerminalU);
}

void LinkPresenter::TermLinkPresenter()
{
	gsDecoTerminalL->Release();
	gsDecoTerminalU->Release();
}
// ====================================================================================================================

LinkPresenter::LinkPresenter(Link* link, ILevelView* levelView)
	:
	Presenter(link, nullptr, levelView)
{
	const LinkJointSet& joints = link->GetJoints();
	for (auto joint : joints)
	{
		mJoints[joint] = new JointPresenter(joint, this, levelView);
		mChildren.insert(mJoints[joint]);
	}

	const LinkSegmentSet& segments = link->GetSegments();
	for (auto segment : segments)
	{
		mSegments[segment] = new SegmentPresenter(segment, this, levelView);
		mChildren.insert(mSegments[segment]);
	}

	mLastTime = -1;
	mSourceJoint = link->GetSourceJoint(ComponentSet());

	UpdateBound();

	mComponent->RegisterListener(static_cast<IComponentListener*>(this));
}

LinkPresenter::~LinkPresenter()
{
	mComponent->UnregisterListener(static_cast<IComponentListener*>(this));
}

PresenterType LinkPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_LINK;
}

void LinkPresenter::OnChildAddition(const ComponentSet& components)
{
	for (auto component : components)
	{
		assert(component->GetParent() == mComponent);
		Connectable* connectable = static_cast<Connectable*>(component);
		if (connectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_JOINT)
		{
			LinkJoint* joint = static_cast<LinkJoint*>(connectable);
			mJoints[joint] = new JointPresenter(joint, this, mLevelView);
			mChildren.insert(mJoints[joint]);
		}
		else
		{
			assert(connectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_SEGMENT);
			LinkSegment* segment = static_cast<LinkSegment*>(connectable);
			mSegments[segment] = new SegmentPresenter(segment, this, mLevelView);
			mChildren.insert(mSegments[segment]);
		}
	}

	auto link = static_cast<Link*>(mComponent);
	mSourceJoint = link->GetSourceJoint(ComponentSet());
}

void LinkPresenter::OnChildRemoval(const ComponentSet& components)
{
	for (auto component : components)
	{
		assert(component->GetParent() == mComponent);
		Connectable* connectable = static_cast<Connectable*>(component);
		if (connectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_JOINT)
		{
			LinkJoint* joint = static_cast<LinkJoint*>(connectable);
			ComponentToJointPresenter::const_iterator jit = mJoints.find(joint);
			assert(jit != mJoints.end());
			JointPresenter* presenter = jit->second;

			mLevelView->InvalidateInteractionFor(presenter);

			mJoints.erase(joint);
			mChildren.erase(presenter);
		}
		else
		{
			assert(connectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_SEGMENT);
			LinkSegment* segment = static_cast<LinkSegment*>(connectable);
			ComponentToSegmentPresenter::const_iterator sit = mSegments.find(segment);
			assert(sit != mSegments.end());
			SegmentPresenter* presenter = sit->second;

			mLevelView->InvalidateInteractionFor(presenter);
			
			mSegments.erase(segment);
			mChildren.erase(presenter);
		}
	}

	auto link = static_cast<Link*>(mComponent);
	mSourceJoint = link->GetSourceJoint(components);
}

void LinkPresenter::UpdateBound()
{
	mBound.MakeInvalid();

	for (auto joint : mJoints)
	{
		joint.second->UpdateBound();
		mBound.Combine(joint.second->GetBound());
	}

	for (auto segment : mSegments)
	{
		segment.second->UpdateBound();
		mBound.Combine(segment.second->GetBound());
	}
}

void LinkPresenter::CollectWith(const MPRectangle& rect, PresenterSet& containees)
{
	for (auto& child : mChildren)
	{
		child->CollectWith(rect, containees);
	}
}

Presenter* LinkPresenter::FindHit(const MPVector& point) const
{
	if (false == mBound.Contains(point))
	{
		return nullptr;
	}

	for (const auto joint : mJoints)
	{
		Presenter* hit = joint.second->FindHit(point);
		if (hit) return hit;
	}

	for (const auto segment : mSegments)
	{
		Presenter* hit = segment.second->FindHit(point);
		if (hit) return hit;
	}

	return nullptr;
}

void LinkPresenter::BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets)
{
	for (const auto& child : mChildren)
	{
		child->BuildSnapTargets(pointTargets, segmentTargets);
	}
}

void LinkPresenter::DrawCandidate(ID2D1DeviceContext* context, MPVector source, MPVector target, bool targetPortExists)
{
	static vector<MPVector> sLinkPath;

	sLinkPath.clear();
	Link::GetOptimalPathFor(source, target, sLinkPath);

	int numPoints = static_cast<int>(sLinkPath.size()) - 1;
	if (numPoints == 0)
	{
		return;
	}

	ID2D1Bitmap1* terminal;
	if (targetPortExists)
	{
		solidBrush->SetColor(HOT_COLOR);
		terminal = gsDecoTerminalL;
	}
	else
	{
		solidBrush->SetColor(SELECTED_COLOR);
		terminal = gsDecoTerminalU;
	}

	for (int i = 0; i < numPoints; ++i)
	{
		D2D1_POINT_2F pt0 = sLinkPath[  i  ].ToPointF();
		D2D1_POINT_2F pt1 = sLinkPath[i + 1].ToPointF();
		context->DrawLine(pt0, pt1, solidBrush, 1, dashedStrokeStyle);
	}

	D2D1_RECT_F terminalRect;

	terminalRect.left = static_cast<FLOAT>(source.x - 6);
	terminalRect.top = static_cast<FLOAT>(source.y - 6);
	terminalRect.right = static_cast<FLOAT>(source.x + 5);
	terminalRect.bottom = static_cast<FLOAT>(source.y + 5);
	context->DrawBitmap(terminal, terminalRect);

	terminalRect.left = static_cast<FLOAT>(target.x - 6);
	terminalRect.top = static_cast<FLOAT>(target.y - 6);
	terminalRect.right = static_cast<FLOAT>(target.x + 5);
	terminalRect.bottom = static_cast<FLOAT>(target.y + 5);
	context->DrawBitmap(terminal, terminalRect);
}

void LinkPresenter::DrawMoveGhost(ID2D1DeviceContext* context)
{
	solidBrush->SetColor(NORMAL_COLOR);

	ComponentToSegmentPresenter::const_iterator sit;
	for (sit = mSegments.begin(); sit != mSegments.end(); ++sit)
	{
		sit->second->DrawMoveGhost(context);
	}

	ComponentToJointPresenter::const_iterator jit;
	for (jit = mJoints.begin(); jit != mJoints.end(); ++jit)
	{
		jit->second->DrawMoveGhost(context);
	}
}

void LinkPresenter::Render(ID2D1DeviceContext* context, double t)
{
	for (auto segment : xsc::reverse(mSegments))
	{
		segment.second->Render(context, t);
	}

	if (mLastTime < 0)
	{
		mFlowPosition = 0;
	}
	else
	{
		static const double MARKER_FLOW_SPEED = 30;
		double deltaT = t - mLastTime;
		mFlowPosition += MARKER_FLOW_SPEED * deltaT;
	}
	mLastTime = t;

	assert(mSourceJoint);
	double travelled = 0;
	auto nextConnectable = mSourceJoint->GetDestination();
	while (nextConnectable->GetConnectableType() != CONNECTABLE_TYPE_PORT)
	{
		if (nextConnectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_SEGMENT)
		{
			auto iter = mSegments.find(nextConnectable);
			assert(iter != mSegments.end());
			if (iter->second->DrawFlowMarker(context, mFlowPosition, travelled))
			{
				break;
			}
			else
			{
				nextConnectable = (static_cast<LinkSegment*>(nextConnectable))->GetDestinationJoint();
			}
		}
		else
		{
			assert(nextConnectable->GetConnectableType() == CONNECTABLE_TYPE_LINK_JOINT);
			nextConnectable = (static_cast<LinkJoint*>(nextConnectable))->GetDestination();
		}		
	}
	if (mFlowPosition >= travelled)
	{
		mFlowPosition = 0;
	}

	for (auto joint : xsc::reverse(mJoints))
	{
		joint.second->Render(context, t);
	}
}

} // namespace hspr
