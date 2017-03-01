#include "hsmnPrecompiled.h"
#include "hsmnDHSelectGroup.h"
#include "hsmnLevelView.h"
#include <hsprBlockPresenter.h>
#include <hsprJointPresenter.h>
#include <hsprSegmentPresenter.h>

#undef max
#undef min

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

GroupSelectionMode DHSelectGroup::groupSelectionMode = GSM_ALL;

DHSelectGroup::DHSelectGroup(LevelView* view) : DragHandler(view)
{
}

DHSelectGroup::~DHSelectGroup()
{
}

void DHSelectGroup::Enter(CPoint point)
{
	mLatent = true;

	mPivotPoint = mView->ViewToWorld({ point.x, point.y });
	mCurrentPoint = mPivotPoint;
	mRect.l = mRect.r = mPivotPoint.x;
	mRect.t = mRect.b = mPivotPoint.y;
}

void DHSelectGroup::Leave(CPoint point)
{
	if (mLatent) return;

	mCurrentPoint = mView->ViewToWorld({ point.x, point.y });

	UpdateRect();

	PresenterSet selections;
	for (const auto& presenter : mView->mPresenters)
	{
		presenter.second->CollectWith(mRect, selections);
	}

	if (groupSelectionMode == GSM_BLOCK)
	{
		for (auto presenter : selections)
		{
			if (presenter->GetPresenterType() == PRESENTER_TYPE_BLOCK)
			{
				mView->mSelectedPresenters.insert(presenter);
			}
		}
	}
	else if (groupSelectionMode == GSM_JOINT)
	{
		for (auto presenter : selections)
		{
			if (presenter->GetPresenterType() == PRESENTER_TYPE_JOINT)
			{
				mView->mSelectedPresenters.insert(presenter);
			}
		}
	}
	else if (groupSelectionMode == GSM_SEGMENT)
	{
		for (auto presenter : selections)
		{
			if (presenter->GetPresenterType() == PRESENTER_TYPE_SEGMENT)
			{
				mView->mSelectedPresenters.insert(presenter);
			}
		}
	}
	else if (groupSelectionMode == GSM_ALL)
	{
		mView->mSelectedPresenters = std::move(selections);
	}

	for (auto presenter : mView->mSelectedPresenters)
	{
		auto selectable = presenter->GetSelectableInterface();
		selectable->SetSelected(true);

		if (mView->mPrimaryBlock == nullptr && presenter->GetPresenterType() == PRESENTER_TYPE_BLOCK)
		{
			mView->ResetPrimary(presenter);
		}
	}

	mView->ReportActiveSelections();
}

void DHSelectGroup::UpdateRect()
{
	int xMin = std::min(mPivotPoint.x, mCurrentPoint.x);
	int xMax = std::max(mPivotPoint.x, mCurrentPoint.x);
	int yMin = std::min(mPivotPoint.y, mCurrentPoint.y);
	int yMax = std::max(mPivotPoint.y, mCurrentPoint.y);
	mRect.Set(xMin, yMin, xMax, yMax);
}

void DHSelectGroup::OnMove(CPoint point)
{
	mCurrentPoint = mView->ViewToWorld({ point.x, point.y });
	MPVector delta(mCurrentPoint - mPivotPoint);
	if (mLatent)
	{
		if (std::abs(delta.x) > Presenter::POINTER_ACTION_LATENCY ||
			std::abs(delta.y) > Presenter::POINTER_ACTION_LATENCY)
		{
			mLatent = false;
		}
		else
		{
			return;
		}
	}

	UpdateRect();
}

void DHSelectGroup::OnDrawTop()
{
	if (mLatent) return;

	ID2D1DeviceContext* context = mView->mRenderer.GetContext();
	Presenter::DrawSelectionRect(context, mRect);
}

} // namespace hsmn
