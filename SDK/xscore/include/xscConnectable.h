#pragma once

#include <xscComponent.h>

namespace xsc {

class Connectable;
typedef Pointer<Connectable> ConnectablePtr;
typedef std::list<Connectable*> ConnectableList;
typedef std::vector<Connectable*> ConnectableVec;
typedef std::unordered_set<Connectable*> ConnectableSet;

enum ConnectableType
{
	CONNECTABLE_TYPE_PORT,
	CONNECTABLE_TYPE_LINK_JOINT,
	CONNECTABLE_TYPE_LINK_SEGMENT
};

class XSC_API Connectable : public Component
{
	XSC_DECLARE_DYNAMIC(Connectable)

public:
	virtual ~Connectable();
	virtual ConnectableType GetConnectableType() const = 0;

protected:
	Connectable();
};

} // namespace xsc
