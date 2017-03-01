#pragma once

#include <xscPort.h>

namespace xsc {

class InputPort;
typedef Pointer<InputPort> InputPortPtr;
typedef std::list<InputPort*> InputPortList;
typedef std::vector<InputPort*> InputPortVec;
typedef std::unordered_set<InputPort*> InputPortSet;

class XSC_API InputPort : public Port
{
	XSC_DECLARE_DYNCREATE(InputPort)

public:
	virtual ~InputPort();

	virtual PortType GetPortType() const override;

	virtual void Connect(Connectable* connectable) override;
	virtual void Disconnect(Connectable* connectable) override;
	virtual void DisconnectAll() override;

protected:
	InputPort();

public:
	static void Init_InputPort();
	static void Term_InputPort();
};

} // namespace xsc
