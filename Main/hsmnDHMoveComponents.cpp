#include "hsmnPrecompiled.h"
#include "hsmnDHMoveComponents.h"
#include "hsmnLevelView.h"
#include <hsmoModel.h>
#include <hsmoMoveCommand.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

DHMoveComponents::DHMoveComponents(LevelView* view)
	:
	DragHandler(view)
{
}

DHMoveComponents::~DHMoveComponents()
{
}

void DHMoveComponents::Enter(CPoint point)
{
	mLatent = true;
	mPivotPoint = { point.x, point.y };
	mCurrentPoint = mPivotPoint;

	mComponents.clear();
	mComponents.reserve(mView->mSelectedPresenters.size());
	for (const auto presenter : mView->mSelectedPresenters)
	{
		mComponents.insert(presenter->GetComponent());
	}

	mPointSnapTargets.clear();
	mSegmentSnapTargets.clear();
	for (const auto& presenter : mView->mPresenters)
	{
		presenter.second->BuildSnapTargets(mPointSnapTargets, mSegmentSnapTargets);
	}

	mSnapResult.Clear();
}

void DHMoveComponents::Leave(CPoint point)
{
	if (mLatent) return;

	mCurrentPoint = { point.x, point.y };
	MPVector delta(mCurrentPoint - mPivotPoint);

	mSnapResult = mView->mHotPresenter->FindSnap(delta, mPointSnapTargets, mSegmentSnapTargets);
	if (mSnapResult.presenter)
	{
		delta += mSnapResult.offset;
	}

	if (delta.IsNotZero())
	{
		MoveCommand* command = new MoveCommand(mView->mLevel, std::move(mComponents), delta);
		Model* model = static_cast<Model*>(mView->mLevel->GetModel());
		model->ExecuteCommand(command);
	}
}

void DHMoveComponents::OnMove(CPoint point)
{
	mCurrentPoint = { point.x, point.y };

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

	mSnapResult = mView->mHotPresenter->FindSnap(delta, mPointSnapTargets, mSegmentSnapTargets);
}

void DHMoveComponents::OnDrawBottom()
{
}

void DHMoveComponents::OnDrawTop()
{
	if (mLatent) return;

	ID2D1DeviceContext* context = mView->mRenderer.GetContext();

	D2D1_MATRIX_3X2_F transform;
	context->GetTransform(&transform);

	MPVector delta(mCurrentPoint - mPivotPoint);
	D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Identity();
	translate._31 = static_cast<FLOAT>(delta.x);
	translate._32 = static_cast<FLOAT>(delta.y);
	context->SetTransform(translate * transform);

	for (auto presenter : mView->mSelectedPresenters)
	{
		presenter->DrawMoveGhost(context);
	}

	context->SetTransform(transform);

	if (mSnapResult.presenter)
	{
		mSnapResult.presenter->DrawSnapEffect(context);
	}
}

} // namespace hsmn
