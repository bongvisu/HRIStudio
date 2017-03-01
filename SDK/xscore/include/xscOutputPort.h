#pragma once

#include <xscPort.h>

namespace xsc {

class OutputPort;
typedef Pointer<OutputPort> OutputPortPtr;
typedef std::list<OutputPort*> OutputPortList;
typedef std::vector<OutputPort*> OutputPortVec;
typedef std::unordered_set<OutputPort*> OutputPortSet;

class XSC_API OutputPort : public Port
{
	XSC_DECLARE_DYNCREATE(OutputPort)

public:
	virtual ~OutputPort();

	virtual PortType GetPortType() const override;

	virtual void Connect(Connectable* connectable) override;
	virtual void Disconnect(Connectable* connectable) override;
	virtual void DisconnectAll() override;

protected:
	OutputPort();

public:
	static void Init_OutputPort();
	static void Term_OutputPort();
};

} // namespace xsc
