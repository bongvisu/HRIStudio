#pragma once

#include <xscComponent.h>

namespace xsc {

class XSC_API INotiListener
{
public:
	virtual void OnNotiChildAddition(Component* sender, const ComponentSet& components) = 0;
	virtual void OnNotiChildRemoval(Component* sender, const ComponentSet& components) = 0;
	virtual void OnNotiPropertyChange(Component* sender, Entity::PropID prop) = 0;

protected:
	virtual ~INotiListener();
};

class XSC_API NotiCenter
{
public:
	void RegisterListener(INotiListener* listener);
	void UnregisterListener(INotiListener* listener);

private:
	NotiCenter();
	~NotiCenter();

	friend Component;
	void PropagateChildAddition(Component* sender, const ComponentSet& components);
	void PropagateChildRemoval(Component* sender, const ComponentSet& components);
	void PropagatePropertyChange(Component* sender, Entity::PropID prop);

private:
	std::unordered_set<INotiListener*> mListeners;

public:
	static void Init_NotiCenter();
	static void Term_NotiCenter();
};

extern XSC_API NotiCenter* theNotiCenter;

} // namespace xsc
