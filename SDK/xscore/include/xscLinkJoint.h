#pragma once

#include <xscLinkElement.h>
#include <xscILinkJoint.h>

namespace xsc {

class LinkJoint;
typedef Pointer<LinkJoint> LinkJointPtr;
typedef std::list<LinkJoint*> LinkJointList;
typedef std::vector<LinkJoint*> LinkJointVec;
typedef std::unordered_set<LinkJoint*> LinkJointSet;

class XSC_API LinkJoint
	:
	public LinkElement,
	public ILinkJoint
{
	XSC_DECLARE_DYNCREATE(LinkJoint)

public:
	static const PropID PROP_LOCATION = LinkElement::LAST_PROP;
	static const PropID LAST_PROP = PROP_LOCATION + 1;

public:
	LinkJoint();
	virtual ~LinkJoint();

	virtual ConnectableType GetConnectableType() const override;

	virtual Connectable* GetSource() const override;
	virtual Connectable* GetDestination() const override;
	virtual const MPVector& GetLocation() const override;
	
	bool IsAnchor() const;
	int GetNumConnectedPorts() const;
	int GetNumConnectedSegments() const;

	void GetConnectedPorts(ConnectableVec& connectables) const;
	void GetConnectedPorts(ConnectableSet& connectables) const;

	void GetConnectedSegments(ConnectableVec& connectables) const;
	void GetConnectedSegments(ConnectableSet& connectables) const;

	void Move(const MPVector& delta);
	void SynchronizeLocationToPort();

	void ConnectSource(Connectable* connectable);
	void ConnectDestination(Connectable* connectable);
	void DisconnectSource();
	void DisconnectDestination();
	void DisconnectPorts();

	virtual void SetSource(Connectable* connectable) override;
	virtual void SetDestination(Connectable* connectable) override;
	virtual void SetLocation(const MPVector& location) override;

	void VerifyForIsolation(ComponentSet& remove);
	virtual void Isolate() override;
	virtual void InvalidateUpward(Connectable* originator, ComponentSet& remove) override;
	virtual void InvalidateDownward(Connectable* originator, ComponentSet& remove) override;

	class XSC_API LinkJointMemento : public ComponentMemento
	{
	public:
		LinkJointMemento(LinkJoint* joint);
		virtual void Restore() override;

	protected:
		MPVector mLocation;
		Connectable* mSource;
		Connectable* mDestination;
	};
	virtual MementoPtr SaveState() const override;

	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;

	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;

protected:
	MPVector mLocation;
	Connectable* mSource;
	Connectable* mDestination;

public:
	static void Init_LinkJoint();
	static void Term_LinkJoint();
};

} // namespace xsc
