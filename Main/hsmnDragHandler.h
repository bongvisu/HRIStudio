#pragma once

#include <xscMPVector.h>

namespace hsmn {

class LevelView;

enum DragHandlerType
{
	DRAG_HANDLER_TYPE_SELECT_GROUP = 0,
	DRAG_HANDLER_TYPE_MOVE_COMPONENTS,
	DRAG_HANDLER_TYPE_RESIZE_BLOCK,
	DRAG_HANDLER_TYPE_ADD_LINK,
	NUMBER_OF_DRAG_HANDLER_TYPES
};

class DragHandler
{
public:
	virtual ~DragHandler();

	virtual void Enter(CPoint point);
	virtual void Leave(CPoint point);
	
	virtual void OnMove(CPoint point);
	
	virtual void OnDrawBottom();
	virtual void OnDrawTop();

protected:
	DragHandler(LevelView* view);
	
protected:
	LevelView* mView;

	bool mLatent;
	xsc::MPVector mPivotPoint;
};

} // namespace hsmn
