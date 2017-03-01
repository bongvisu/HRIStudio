#pragma once

namespace hsmn {

class ILoopConsumer
{
public:
	virtual void DoLoop(double t) = 0;

protected:
	virtual ~ILoopConsumer();
};

} // namespace hsmn
