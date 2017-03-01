#pragma once

#include <xscObject.h>

namespace xsc {

class Entity;
typedef Pointer<Entity> EntityPtr;
typedef std::list<Entity*> EntityList;
typedef std::vector<Entity*> EntityVec;
typedef std::unordered_set<Entity*> EntitySet;

class XSC_API Entity : public Object
{
	XSC_DECLARE_DYNAMIC(Entity)

public:
	typedef uint64_t ID;
	static const ID NULL_ID = 0;
	static Entity* FromID(ID id);
	
	typedef uint32_t PropID;
	static const PropID LAST_PROP = 0;

public:
	ID GetID() const;
	virtual ~Entity();

protected:
	Entity();

protected:
	ID mID;
};

} // namespace xsc
