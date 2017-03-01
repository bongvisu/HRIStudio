#pragma once

#include <hsprPresenter.h>
#include "hsmnDragHandler.h"

namespace hsmn {

class DHMoveComponents : public DragHandler
{
public:
	DHMoveComponents(LevelView* view);
	virtual ~DHMoveComponents();

protected:
	virtual void Enter(CPoint point) override;
	virtual void Leave(CPoint point) override;

	virtual void OnMove(CPoint point) override;
	virtual void OnDrawBottom() override;
	virtual void OnDrawTop() override;

	xsc::ComponentSet mComponents;

protected:
	xsc::MPVector mCurrentPoint;

	hspr::PresenterVec mPointSnapTargets;
	hspr::PresenterVec mSegmentSnapTargets;

	hspr::Presenter::SnapResult mSnapResult;
};

} // namespace hsmn
