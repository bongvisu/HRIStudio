#pragma once

#include <hsprPortPresenter.h>
#include "hsmnDragHandler.h"

namespace hsmn {

class DHAddLink : public DragHandler
{
public:
	DHAddLink(LevelView* view);
	virtual ~DHAddLink();

protected:
	virtual void Enter(CPoint point) override;
	virtual void Leave(CPoint point) override;

	virtual void OnMove(CPoint point) override;
	virtual void OnDrawTop() override;

	void UpdateTargetData(CPoint point);

protected:
	hspr::PortPresenter*	mSourcePort;
	xsc::PortType			mSourceType;
	xsc::MPVector			mSourcePoint;
	
	xsc::MPVector			mTargetPoint;
	hspr::PortPresenter*	mTargetPort;
};

} // namespace hsmn
