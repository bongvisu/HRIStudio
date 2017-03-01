#pragma once

#include <xscConnectable.h>

namespace xsc {

class LinkElement;
typedef Pointer<LinkElement> LinkElementPtr;
typedef std::list<LinkElement*> LinkElementList;
typedef std::vector<LinkElement*> LinkElementVec;
typedef std::unordered_set<LinkElement*> LinkElementSet;

class XSC_API LinkElement : public Connectable
{
	XSC_DECLARE_DYNAMIC(LinkElement)

public:
	virtual ~LinkElement();

	virtual void Isolate();
	virtual void InvalidateUpward(Connectable* originator, ComponentSet& remove);
	virtual void InvalidateDownward(Connectable* originator, ComponentSet& remove);

protected:
	LinkElement();
};

} // namespace xsc
