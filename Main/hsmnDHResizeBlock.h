#pragma once

#include <hsprBlockPresenter.h>
#include "hsmnDragHandler.h"

namespace hsmn {

class DHResizeBlock : public DragHandler
{
public:
	DHResizeBlock(LevelView* view);
	virtual ~DHResizeBlock();

protected:
	virtual void Enter(CPoint point) override;
	virtual void Leave(CPoint point) override;
	
	virtual void OnMove(CPoint point) override;
	virtual void OnDrawTop() override;

protected:
	xsc::MPVector mCurrentPoint;
	hspr::BlockPresenter* mBlock;
};

} // namespace hsmn
