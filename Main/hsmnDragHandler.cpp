#include "hsmnPrecompiled.h"
#include "hsmnDragHandler.h"
#include "hsmnLevelView.h"

namespace hsmn {

DragHandler::DragHandler(LevelView* view)
	:
	mView(view)
{
}

DragHandler::~DragHandler()
{
}

void DragHandler::Enter(CPoint point)
{
}

void DragHandler::Leave(CPoint point)
{
}

void DragHandler::OnMove(CPoint point)
{
}

void DragHandler::OnDrawBottom()
{
}

void DragHandler::OnDrawTop()
{
}

} // namespace hsmn
