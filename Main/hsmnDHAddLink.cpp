#include "hsmnPrecompiled.h"
#include "hsmnDHAddLink.h"
#include "hsmnLevelView.h"
#include <hsmoModel.h>
#include <hsmoAddLinkCommand.h>
#include <hsprLinkPresenter.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

DHAddLink::DHAddLink(LevelView* view)
	:
	DragHandler(view)
{
}

DHAddLink::~DHAddLink()
{
}

void DHAddLink::Enter(CPoint point)
{
	mLatent = true;
	mPivotPoint = { point.x, point.y };
	 
	mSourcePort = static_cast<PortPresenter*>(mView->mHotPresenter);
	mSourceType = mSourcePort->GetPortType();
	mSourcePoint = mSourcePort->GetConnectionPoint();
}

void DHAddLink::Leave(CPoint point)
{
	if (mLatent) return;

	UpdateTargetData(point);
	if (mTargetPort)
	{
		OutputPort* source;
		InputPort* target;
		if (mSourceType == PORT_TYPE_OUTPUT)
		{
			source = static_cast<OutputPort*>(mSourcePort->GetComponent());
			target = static_cast<InputPort*>(mTargetPort->GetComponent());
		}
		else
		{
			source = static_cast<OutputPort*>(mTargetPort->GetComponent());
			target = static_cast<InputPort*>(mSourcePort->GetComponent());
		}

		AddLinkCommand* command = new AddLinkCommand(mView->mLevel, source, target);
		Model* model = static_cast<Model*>(mView->mLevel->GetModel());
		model->ExecuteCommand(command);
	}
}

void DHAddLink::OnMove(CPoint point)
{
	if (mLatent)
	{
		int dx = std::abs(point.x - mPivotPoint.x);
		int dy = std::abs(point.y - mPivotPoint.y);
		if (dx > Presenter::POINTER_ACTION_LATENCY || dy > Presenter::POINTER_ACTION_LATENCY)
		{
			mLatent = false;
		}
		else
		{
			return;
		}
	}

	UpdateTargetData(point);
}

void DHAddLink::UpdateTargetData(CPoint point)
{
	mTargetPort = nullptr;
	mTargetPoint = mView->ViewToWorld({ point.x, point.y });

	mView->UpdateHotPresenter(point);
	if (nullptr == mView->mHotPresenter || mView->mHotPresenter == mSourcePort) return;
	if (mView->mHotPresenter->GetPresenterType() != PRESENTER_TYPE_PORT) return;
	PortPresenter* port = static_cast<PortPresenter*>(mView->mHotPresenter);
	if (port->IsOccupied() || port->GetPortType() == mSourceType) return;

	mTargetPort = port;
	mTargetPoint = mTargetPort->GetConnectionPoint();
}

void DHAddLink::OnDrawTop()
{
	if (mLatent) return;
	
	ID2D1DeviceContext* context = mView->mRenderer.GetContext();
	bool targetExists = mTargetPort != nullptr;
	LinkPresenter::DrawCandidate(context, mSourcePoint, mTargetPoint, targetExists);
}

} // namespace hsmn
