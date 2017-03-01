#include "hsmnPrecompiled.h"
#include "hsmnDHResizeBlock.h"
#include "hsmnLevelView.h"
#include <hsmoModel.h>
#include <hsmoResizeBlockCommand.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

DHResizeBlock::DHResizeBlock(LevelView* view)
	:
	DragHandler(view)
{
}

DHResizeBlock::~DHResizeBlock()
{
}

void DHResizeBlock::Enter(CPoint point)
{
	assert(mView->mHotPresenter->GetPresenterType() == PRESENTER_TYPE_CORNER);
	CornerPresenter* corner = static_cast<CornerPresenter*>(mView->mHotPresenter);
	mBlock = static_cast<BlockPresenter*>(corner->GetParent());

	mLatent = true;
	mPivotPoint = mView->ViewToWorld({ point.x, point.y });
	mCurrentPoint =  mPivotPoint;
	
	mBlock->SetupForResize(corner->GetCorner());
}

void DHResizeBlock::Leave(CPoint point)
{
	if (mLatent) return;

	mCurrentPoint = mView->ViewToWorld({ point.x, point.y });
	MPVector delta(mCurrentPoint - mPivotPoint);
	mBlock->UpdateResizeRect(delta);

	Block* block = static_cast<Block*>(mBlock->GetComponent());
	const auto& resizeGeometry = mBlock->GetResizeGeometry();
	if (resizeGeometry != block->GetGeometry())
	{
		ResizeBlockCommand* command = new ResizeBlockCommand(mView->mLevel, block, resizeGeometry);
		Model* model = static_cast<Model*>(block->GetModel());
		model->ExecuteCommand(command);
	}
}

void DHResizeBlock::OnMove(CPoint point)
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

	mBlock->UpdateResizeRect(delta);
}

void DHResizeBlock::OnDrawTop()
{
	if (mLatent) return;
	
	mBlock->DrawResizeGhost(mView->mRenderer.GetContext());
}

} // namespace hsmn
