#pragma once

#include <xscObject.h>

namespace xsc {

class Memento;
typedef Pointer<Memento> MementoPtr;
typedef std::vector<MementoPtr> MementoPtrVec;

class XSC_API Memento : public Object
{
public:
	virtual ~Memento();

	virtual void Restore();

protected:
	Memento();
};

} // namespace xsc
