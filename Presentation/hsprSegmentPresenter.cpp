#include "hsprPrecompiled.h"
#include "hsprSegmentPresenter.h"
#include "hsprJointPresenter.h"
#include "hsprResource.h"
#include <xscLink.h>
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static ID2D1Bitmap1* gsFlowXP;
static ID2D1Bitmap1* gsFlowXN;
static ID2D1Bitmap1* gsFlowYP;
static ID2D1Bitmap1* gsFlowYN;

void SegmentPresenter::InitSegmentPresenter()
{
	WicBitmap wicBitmap;
	auto context = D2DRenderer::resourcingContext;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_FLOW_X_POSITIVE);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsFlowXP);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_FLOW_X_NEGATIVE);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsFlowXN);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_FLOW_Y_POSITIVE);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsFlowYP);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_FLOW_Y_NEGATIVE);
	context->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsFlowYN);
}

void SegmentPresenter::TermSegmentPresenter()
{
	gsFlowXP->Release();
	gsFlowXN->Release();
	gsFlowYP->Release();
	gsFlowYN->Release();
}
// ====================================================================================================================

SegmentPresenter::SegmentPresenter(LinkSegment* segment, Presenter* parent, ILevelView* levelView)
	:
	Presenter(segment, parent, levelView),
	mHot(false),
	mSelected(false)
{
	UpdateBound();
}

SegmentPresenter::~SegmentPresenter()
{
}

PresenterType SegmentPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_SEGMENT;
}

IVisual* SegmentPresenter::GetVisualInterface() const
{
	return static_cast<IVisual*>(const_cast<SegmentPresenter*>(this));
}

ISelectable* SegmentPresenter::GetSelectableInterface() const
{
	return static_cast<ISelectable*>(const_cast<SegmentPresenter*>(this));
}

void SegmentPresenter::SetHot(bool hot)
{
	mHot = hot;
}

bool SegmentPresenter::GetHot() const
{
	return mHot;
}

void SegmentPresenter::SetSelected(bool selected)
{
	mSelected = selected;
}

bool SegmentPresenter::GetSelected() const
{
	return mSelected;
}

MPVector SegmentPresenter::GetCrossPoint(const MPVector& approximate) const
{
	return static_cast<LinkSegment*>(mComponent)->GetCrossPoint(approximate);
}

void SegmentPresenter::UpdateBound()
{
	LinkSegment* segment = static_cast<LinkSegment*>(mComponent);
	const MPVector& srcLocation = segment->GetSourceLocation();
	const MPVector& dstLocation = segment->GetDestinationLocation();
	mAlignment = MPVector::GetAlignment(srcLocation, dstLocation);

	static const int SEGMENT_HIT_TOLERANCE = 5;

	if (mAlignment == MPVector::AXIS_X)
	{
		if (srcLocation.x < dstLocation.x)
		{
			mBound.l = srcLocation.x;
			mBound.r = dstLocation.x;
			mLength = dstLocation.x - srcLocation.x;
		}
		else
		{
			mBound.r = srcLocation.x;
			mBound.l = dstLocation.x;
			mLength = srcLocation.x - dstLocation.x;
		}
		mBound.t = srcLocation.y - SEGMENT_HIT_TOLERANCE;
		mBound.b = srcLocation.y + SEGMENT_HIT_TOLERANCE;
	}
	else if (mAlignment == MPVector::AXIS_Y)
	{
		if (srcLocation.y < dstLocation.y)
		{
			mBound.t = srcLocation.y;
			mBound.b = dstLocation.y;
			mLength = dstLocation.y - srcLocation.y;
		}
		else
		{
			mBound.b = srcLocation.y;
			mBound.t = dstLocation.y;
			mLength = srcLocation.y - dstLocation.y;
		}
		mBound.l = srcLocation.x - SEGMENT_HIT_TOLERANCE;
		mBound.r = srcLocation.x + SEGMENT_HIT_TOLERANCE;
	}
}

Presenter* SegmentPresenter::FindHit(const MPVector& point) const
{
	if (mBound.Contains(point))
	{
		return const_cast<SegmentPresenter*>(this);
	}
	return nullptr;
}

void SegmentPresenter::CollectWith(const MPRectangle& rect, PresenterSet& containees)
{
	if (rect.Contains(mBound))
	{
		containees.insert(this);
	}
}

void SegmentPresenter::BuildSnapTargets(PresenterVec& pointTargets, PresenterVec& segmentTargets)
{
	LinkSegment* segment = static_cast<LinkSegment*>(mComponent);
	const MPVector& srcLocation = segment->GetSourceLocation();
	const MPVector& dstLocation = segment->GetDestinationLocation();

	mSnapPoint0.x = std::min(srcLocation.x, dstLocation.x);
	mSnapPoint0.y = std::min(srcLocation.y, dstLocation.y);
	mSnapPoint1.x = std::max(srcLocation.x, dstLocation.x);
	mSnapPoint1.y = std::max(srcLocation.y, dstLocation.y);
	mSnapBound.l = mSnapPoint0.x - POINTER_ACTION_LATENCY;
	mSnapBound.t = mSnapPoint0.y - POINTER_ACTION_LATENCY;
	mSnapBound.r = mSnapPoint1.x + POINTER_ACTION_LATENCY;
	mSnapBound.b = mSnapPoint1.y + POINTER_ACTION_LATENCY;

	segmentTargets.push_back(this);
}

Presenter::SnapResult SegmentPresenter::FindSnap(const MPVector& delta,
	const PresenterVec& pointTargets, const PresenterVec& segmentTargets) const
{
	SnapResult snapResult;

	int spDistance = std::numeric_limits<int>::max();
	float spDistanceFromCenter = std::numeric_limits<float>::max();
	MPVector segmentCenter;
	MPVector point0(mSnapPoint0 + delta);
	MPVector point1(mSnapPoint1 + delta);
	segmentCenter.x = (point0.x + point1.x) / 2;
	segmentCenter.y = (point0.y + point1.y) / 2;

	for (const auto pointTarget : pointTargets)
	{
		IPointSnapTarget* pointST = dynamic_cast<IPointSnapTarget*>(pointTarget);
		if (pointST->GetSnapBound().Intersects(point0, point1))
		{
			if (mAlignment == MPVector::AXIS_X)
			{
				int diff = pointST->GetSnapPoint().y - point0.y;
				int absDiff = std::abs(diff);
				if (absDiff < spDistance)
				{
					spDistance = absDiff;

					snapResult.presenter = pointTarget;
					snapResult.offset.x = 0;
					snapResult.offset.y = diff;
				}
				else if (absDiff == spDistance)
				{
					MPVector cDiff(pointST->GetSnapPoint() - segmentCenter);
					float distance = (float)(cDiff.x * cDiff.x + cDiff.y * cDiff.y);
					if (distance < spDistanceFromCenter)
					{
						spDistanceFromCenter = distance;

						snapResult.presenter = pointTarget;
						snapResult.offset.x = 0;
						snapResult.offset.y = diff;
					}
				}
			}
			else
			{
				int diff = pointST->GetSnapPoint().x - point0.x;
				int absDiff = std::abs(diff);
				if (absDiff < spDistance)
				{
					spDistance = absDiff;

					snapResult.presenter = pointTarget;
					snapResult.offset.x = diff;
					snapResult.offset.y = 0;
				}
				else if (absDiff == spDistance)
				{
					MPVector cDiff(pointST->GetSnapPoint() - segmentCenter);
					float distance = (float)(cDiff.x * cDiff.x + cDiff.y * cDiff.y);
					if (distance < spDistanceFromCenter)
					{
						spDistanceFromCenter = distance;

						snapResult.presenter = pointTarget;
						snapResult.offset.x = diff;
						snapResult.offset.y = 0;
					}
				}
			}
		}
	}

	int ssDistance = std::numeric_limits<int>::max();
	int segmentLength = std::numeric_limits<int>::min();
	for (const auto& segmentTarget : segmentTargets)
	{
		ISegmentSnapTarget* segmentST = dynamic_cast<ISegmentSnapTarget*>(segmentTarget);
		if (segmentST->GetSegmentAlignment() != mAlignment) continue;

		if (segmentST->GetSnapBound().Intersects(point0, point1))
		{
			if (mAlignment == MPVector::AXIS_X)
			{
				int diff = segmentST->GetSnapPoint0().y - point0.y;
				int absDiff = std::abs(diff);
				if (absDiff < ssDistance)
				{
					ssDistance = absDiff;

					snapResult.presenter = segmentTarget;
					snapResult.offset.x = 0;
					snapResult.offset.y = diff;
				}
				else if (absDiff == ssDistance)
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
				int diff = segmentST->GetSnapPoint0().x - point0.x;
				int absDiff = std::abs(diff);
				if (absDiff < ssDistance)
				{
					ssDistance = absDiff;

					snapResult.presenter = segmentTarget;
					snapResult.offset.x = diff;
					snapResult.offset.y = 0;
				}
				else if (absDiff == ssDistance)
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

	return snapResult;
}

int SegmentPresenter::GetSegmentLength() const
{
	return mLength;
}

MPVector::Axis SegmentPresenter::GetSegmentAlignment() const
{
	return mAlignment;
}

const MPVector& SegmentPresenter::GetSnapPoint0() const
{
	return mSnapPoint0;
}

const MPVector& SegmentPresenter::GetSnapPoint1() const
{
	return mSnapPoint1;
}

MPRectangle SegmentPresenter::GetSnapBound() const
{
	return mSnapBound;
}

void SegmentPresenter::DrawSnapEffect(ID2D1DeviceContext* context)
{
	D2D1_ANTIALIAS_MODE antialiasMode = context->GetAntialiasMode();
	context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	solidBrush->SetColor({ 0.5f, 0.5f, 0.7f, 0.3f });

	D2D1_ROUNDED_RECT roundedRect;
	roundedRect.rect.left = static_cast<FLOAT>(mSnapPoint0.x - POINTER_ACTION_LATENCY - 1);
	roundedRect.rect.top = static_cast<FLOAT>(mSnapPoint0.y - POINTER_ACTION_LATENCY - 1);
	roundedRect.rect.right = static_cast<FLOAT>(mSnapPoint1.x + POINTER_ACTION_LATENCY);
	roundedRect.rect.bottom = static_cast<FLOAT>(mSnapPoint1.y + POINTER_ACTION_LATENCY);
	roundedRect.radiusX = roundedRect.radiusY = static_cast<FLOAT>(POINTER_ACTION_LATENCY);
	context->FillRoundedRectangle(roundedRect, solidBrush);

	context->SetAntialiasMode(antialiasMode);
}

void SegmentPresenter::DrawMoveGhost(ID2D1DeviceContext* context)
{
	auto segment = static_cast<LinkSegment*>(mComponent);
	const MPVector& srcLocation = segment->GetSourceLocation();
	const MPVector& dstLocation = segment->GetDestinationLocation();
	MPVector::Axis alignment = segment->GetAlignment();

	int offsetForJoint = JointPresenter::FIXED_SIZE;

	int xMin = std::min(srcLocation.x, dstLocation.x);
	int xMax = std::max(srcLocation.x, dstLocation.x);
	int yMin = std::min(srcLocation.y, dstLocation.y);
	int yMax = std::max(srcLocation.y, dstLocation.y);
	if (alignment == MPVector::AXIS_X)
	{
		xMin += offsetForJoint;
		xMax -= offsetForJoint;
		if (xMax < xMin) xMax = xMin;
	}
	else
	{
		yMin += offsetForJoint;
		yMax -= offsetForJoint;
		if (yMax < yMin) yMax = yMin;
	}
	D2D1_POINT_2F point0, point1;
	point0.x = static_cast<FLOAT>(xMin);
	point0.y = static_cast<FLOAT>(yMin);
	point1.x = static_cast<FLOAT>(xMax);
	point1.y = static_cast<FLOAT>(yMax);

	solidBrush->SetColor(NORMAL_COLOR);
	context->DrawLine(point0, point1, solidBrush, 1, dashedStrokeStyle);
}

bool SegmentPresenter::DrawFlowMarker(ID2D1DeviceContext* context, double target, double& travelled)
{
	travelled += mLength;
	if (target < travelled)
	{
		static const int MARKER_MAJOR_SIZE = 9;
		static const int MARKER_MINOR_SIZE = 5;

		int diff = static_cast<int>(travelled - target);
		auto segment = static_cast<LinkSegment*>(mComponent);
		auto& srcLocation = segment->GetSourceLocation();
		auto& dstLocation = segment->GetDestinationLocation();
		MPVector dirVec(dstLocation - srcLocation);
		auto direction = dirVec.GetDirection();
		MPVector markerPos;
		D2D1_RECT_F rect;
		ID2D1Bitmap1* markerImage;

		if (direction == MPVector::POSITIVE_X)
		{
			int centerPos = dstLocation.x - diff;
			assert(centerPos >= srcLocation.x);

			rect.left = centerPos - 2.0f;
			rect.right = rect.left + MARKER_MINOR_SIZE;
			rect.top = srcLocation.y - 5.0f;
			rect.bottom = rect.top + MARKER_MAJOR_SIZE;
			markerImage = gsFlowXP;
		}
		else if (direction == MPVector::NEGATIVE_X)
		{
			int centerPos = dstLocation.x + diff;
			assert(centerPos <= srcLocation.x);
			
			rect.left = centerPos - 2.0f;
			rect.right = rect.left + MARKER_MINOR_SIZE;
			rect.top = srcLocation.y - 5.0f;
			rect.bottom = rect.top + MARKER_MAJOR_SIZE;
			markerImage = gsFlowXN;
		}
		else if (direction == MPVector::POSITIVE_Y)
		{
			int centerPos = dstLocation.y - diff;
			assert(centerPos >= srcLocation.y);
			
			rect.left = srcLocation.x - 5.0f;
			rect.right = rect.left + MARKER_MAJOR_SIZE;
			rect.top = centerPos - 2.0f;
			rect.bottom = rect.top + MARKER_MINOR_SIZE;
			markerImage = gsFlowYP;
		}
		else if (direction == MPVector::NEGATIVE_Y)
		{
			int centerPos = dstLocation.y + diff;
			assert(centerPos <= srcLocation.y);
			
			rect.left = srcLocation.x - 5.0f;
			rect.right = rect.left + MARKER_MAJOR_SIZE;
			rect.top = centerPos - 2.0f;
			rect.bottom = rect.top + MARKER_MINOR_SIZE;
			markerImage = gsFlowYN;
		}
		context->DrawBitmap(markerImage, rect);
		return true;
	}
	return false;
}

void SegmentPresenter::Render(ID2D1DeviceContext* context, double t)
{
	LinkSegment* segment = static_cast<LinkSegment*>(mComponent);
	const MPVector& srcLocation = segment->GetSourceLocation();
	const MPVector& dstLocation = segment->GetDestinationLocation();

	Link* link = static_cast<Link*>(segment->GetParent());
	assert(link->IsDerivedFrom(XSC_RTTI(Link)));

	ID2D1StrokeStyle1* stroke = nullptr;
	FLOAT targetWidth = (link->GetLineWidth() == LINK_LINE_WIDTH_THIN) ? 1.0f : 3.0f;
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
		if (link->GetLineStyle() == LINK_LINE_STYLE_DASHED)
		{
			stroke = dashDottedStrokeStyle;
		}

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
	context->DrawLine(srcLocation.ToPointF(), dstLocation.ToPointF(), solidBrush, targetWidth, stroke);
}

} // namespace hspr
