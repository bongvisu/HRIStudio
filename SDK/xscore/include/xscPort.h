#pragma once

#include <xscConnectable.h>
#include <xscIPort.h>

namespace xsc {

class Port;
typedef Pointer<Port> PortPtr;
typedef std::list<Port*> PortList;
typedef std::vector<Port*> PortVec;
typedef std::unordered_set<Port*> PortSet;

enum PortType
{
	PORT_TYPE_INPUT,
	PORT_TYPE_OUTPUT
};

class XSC_API Port
	:
	public Connectable,
	public IPort
{
	XSC_DECLARE_DYNAMIC(Port)

public:
	static const int CONNECTION_MARGIN;
	virtual ~Port();

	virtual PortType GetPortType() const = 0;

	virtual ConnectableType GetConnectableType() const override;

	virtual MPVector GetConnectionPoint() const override;
	virtual const ConnectableSet& GetConnectionJoints() const override;
	virtual MPRectangle::Edge GetAttachSide() const override;
	virtual const MPVector& GetLocation() const override;
	
	void Move(const MPVector& delta);

	virtual void AddConnectionJoint(Connectable* connectable) override;
	virtual void RemoveConnectionJoint(Connectable* connection) override;

	virtual void SetAttachSide(MPRectangle::Edge attachSide) override;
	virtual void SetLocation(const MPVector& location) override;

	virtual void Connect(Connectable* connectable) = 0;
	virtual void Disconnect(Connectable* connectable) = 0;
	virtual void DisconnectAll() = 0;

	class XSC_API PortMemento : public ComponentMemento
	{
	public:
		PortMemento(Port* port);
		virtual void Restore() override;

	protected:
		MPVector mLocation;
		MPRectangle::Edge mAttachSide;
		ConnectableSet mConnectionJoints;
	};
	virtual MementoPtr SaveState() const override;

	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;

	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;

protected:
	Port();

protected:
	MPVector mLocation;
	MPRectangle::Edge mAttachSide;
	ConnectableSet mConnectionJoints;
};

} // namespace xsc
