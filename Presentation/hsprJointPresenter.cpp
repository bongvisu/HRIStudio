#include "hsprPrecompiled.h"
#include "hsprJointPresenter.h"
#include "hsprISegmentSnapTarget.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>
#include <xscLink.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

const int JointPresenter::FIXED_SIZE = 2;

JointPresenter::JointPresenter(LinkJoint* joint, Presenter* parent, ILevelView* levelView)
	:
	Presenter(joint, parent, levelView),
	mHot(false),
	mSelected(false)
{
	UpdateBound();
}

JointPresenter::~JointPresenter()
{
}

PresenterType JointPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_JOINT;
}

IVisual* JointPresenter::GetVisualInterface() const
{
	return static_cast<IVisual*>(const_cast<JointPresenter*>(this));
}

ISelectable* JointPresenter::GetSelectableInterface() const
{
	return static_cast<ISelectable*>(const_cast<JointPresenter*>(this));
}

void JointPresenter::SetHot(bool hot)
{
	mHot = hot;
}

bool JointPresenter::GetHot() const
{
	return mHot;
}

void JointPresenter::SetSelected(bool selected)
{
	mSelected = selected;
}

bool JointPresenter::GetSelected() const
{
	return mSelected;
}

MPVector JointPresenter::GetLocation() const
{
	LinkJoint* joint = static_cast<LinkJoint*>(mComponent);
	return joint->GetLocation();
}

void JointPresenter::UpdateBound()
{
	LinkJoint* joint = static_cast<LinkJoint*>(mComponent);
	const MPVector& location = joint->GetLocation();

	static const int JOINT_HIT_TOLERANCE = 5;
	
	mBound.l = location.x - JOINT_HIT_TOLERANCE;
	mBound.r = location.x + JOINT_HIT_TOLERANCE;
	mBound.t = location.y - JOINT_HIT_TOLERANCE;
	mBound.b = location.y + JOINT_HIT_TOLERANCE;
}

Presenter* JointPresenter::FindHit(const xsc::MPVector& point) const
{
	if (mBound.Contains(point))
	{
		return const_cast<JointPresenter*>(this);
	}
	return nullptr;
}

void JointPresenter::CollectWith(const xsc::MPRectangle& rect, PresenterSet& containees)
{
	if (rect.Contains(mBound))
	{
		containees.insert(this);
	}
}

const MPVector& JointPresenter::GetSnapPoint() const
{
	return mSnapPoint;
}

const MPRectangle& JointPresenter::GetSnapBound() const
{
	return mSnapBound;
}

void JointPresenter::BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets)
{
	LinkJoint* joint = static_cast<LinkJoint*>(mComponent);

	mSnapPoint = joint->GetLocation();
	mSnapBound.l = mSnapPoint.x - POINTER_ACTION_LATENCY;
	mSnapBound.t = mSnapPoint.y - POINTER_ACTION_LATENCY;
	mSnapBound.r = mSnapPoint.x + POINTER_ACTION_LATENCY;
	mSnapBound.b = mSnapPoint.y + POINTER_ACTION_LATENCY;

	pointTargets.push_back(this);
}

Presenter::SnapResult JointPresenter::FindSnap(const MPVector& delta,
	const PresenterVec& pointTargets, const PresenterVec& segmentTargets) const
{
	SnapResult snapResult;

	int psDistance = std::numeric_limits<int>::max();
	int segmentLength = std::numeric_limits<int>::min();

	MPVector point(mSnapPoint + delta);

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

	float ppDistance = std::numeric_limits<float>::max();
	for (const auto& pointTarget : pointTargets)
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
	
	return snapResult;
}

void JointPresenter::DrawMoveGhost(ID2D1DeviceContext* context)
{
	LinkJoint* joint = static_cast<LinkJoint*>(mComponent);
	mTempAnchors.clear();
	joint->GetConnectedPorts(mTempAnchors);
	if (mTempAnchors.size() >= 1)
	{
		return;
	}

	const MPVector& location = joint->GetLocation();
	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(location.x - FIXED_SIZE);
	rect.right = static_cast<FLOAT>(location.x + FIXED_SIZE);
	rect.top = static_cast<FLOAT>(location.y - FIXED_SIZE);
	rect.bottom = static_cast<FLOAT>(location.y + FIXED_SIZE);
	context->DrawRectangle(rect, solidBrush, 1, dashedStrokeStyle);
}

void JointPresenter::DrawSnapEffect(ID2D1DeviceContext* context)
{
	D2D1_RECT_F rect;
	rect.left = mSnapPoint.x - 8.0f;
	rect.top = mSnapPoint.y - 8.0f;
	rect.right = mSnapPoint.x + 7.0f;
	rect.bottom = mSnapPoint.y + 7.0f;
	context->DrawBitmap(msTargetImage, rect);
}

void JointPresenter::Render(ID2D1DeviceContext* context, double t)
{
	LinkJoint* joint = static_cast<LinkJoint*>(mComponent);
	const MPVector& location = joint->GetLocation();

	Link* link = static_cast<Link*>(joint->GetParent());
	assert(link->IsDerivedFrom(XSC_RTTI(Link)));

	if (mHot)
	{
		solidBrush->SetColor(HOT_COLOR);
	}
	else if (mSelected)
	{
		solidBrush->SetColor(SELECTED_COLOR);
	}
	else
	{
		switch (link->GetLineColor())
		{
		case LINK_LINE_COLOR_BLACK:
			solidBrush->SetColor(NORMAL_COLOR);
			break;
		case LINK_LINE_COLOR_TEAL:
			solidBrush->SetColor(TEAL_COLOR);
			break;
		case LINK_LINE_COLOR_PURPLE:
			solidBrush->SetColor(PURPLE_COLOR);
			break;
		}
	}

	static const int COMMON_IMAGE_SIZE = 9;

	mTempAnchors.clear();
	joint->GetConnectedPorts(mTempAnchors);

	if (mTempAnchors.size() >= 1)
	{
		return;
	}

	if (mHot || mSelected)
	{
		D2D1_RECT_F rect;
		rect.left = static_cast<FLOAT>(location.x - FIXED_SIZE - 1);
		rect.right = static_cast<FLOAT>(location.x + FIXED_SIZE);
		rect.top = static_cast<FLOAT>(location.y - FIXED_SIZE - 1);
		rect.bottom = static_cast<FLOAT>(location.y + FIXED_SIZE);
		context->FillRectangle(rect, solidBrush);
	}
	else
	{
		D2D1_RECT_F rect;
		if (link->GetLineWidth() == LINK_LINE_WIDTH_THIN)
		{
			rect.left = static_cast<FLOAT>(location.x - 1);
			rect.right = static_cast<FLOAT>(location.x);
			rect.top = static_cast<FLOAT>(location.y - 1);
			rect.bottom = static_cast<FLOAT>(location.y);
		}
		else
		{
			rect.left = static_cast<FLOAT>(location.x - 2);
			rect.right = static_cast<FLOAT>(location.x + 1);
			rect.top = static_cast<FLOAT>(location.y - 2);
			rect.bottom = static_cast<FLOAT>(location.y + 1);
		}
		context->FillRectangle(rect, solidBrush);
	}
}

} // namespace hspr
