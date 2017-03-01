#pragma once

#include <xscMPRectangle.h>
#include "hsmnDragHandler.h"

namespace hsmn {

enum GroupSelectionMode
{
	GSM_BLOCK = 0,
	GSM_JOINT,
	GSM_SEGMENT,
	GSM_ALL
};

class DHSelectGroup : public DragHandler
{
public:
	static GroupSelectionMode groupSelectionMode;

public:
	DHSelectGroup(LevelView* view);
	virtual ~DHSelectGroup();

protected:
	void Enter(CPoint point) override;
	void Leave(CPoint point) override;

	void OnMove(CPoint point) override;
	void OnDrawTop() override;

	void UpdateRect();

protected:
	xsc::MPVector mCurrentPoint;
	xsc::MPRectangle mRect;
};

} // namespace hsmn
