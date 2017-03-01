#pragma once

namespace hsmn {

class IBlockDropTarget
{
public:
	virtual void OnBlockDrop(const wchar_t* rttiName, CPoint point/*screen space*/) = 0;

protected:
	virtual ~IBlockDropTarget();
};

} // namespace hsmn
