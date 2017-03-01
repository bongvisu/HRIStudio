#pragma once

#include <xscLink.h>

namespace xsc {

class Block;
typedef Pointer<Block> BlockPtr;
typedef std::list<Block*> BlockList;
typedef std::vector<Block*> BlockVec;
typedef std::unordered_set<Block*> BlockSet;

class XSC_API Block : public Component
{
	XSC_DECLARE_DYNAMIC(Block)

public:
	static const PropID PROP_GEOMETRY = Component::LAST_PROP;
	static const PropID LAST_PROP = PROP_GEOMETRY + 1;

public:
	void Construct(const MPVector& center);
	virtual ~Block();

	const MPRectangle& GetGeometry() const;

	int GetNumInputPorts() const;
	int GetNumOutputPorts() const;
	InputPort* GetInputPortAt(int index) const;
	OutputPort* GetOutputPortAt(int index) const;
	const InputPortVec& GetInputPorts() const;
	const OutputPortVec& GetOutputPorts() const;
	const PortVec& GetPorts(MPRectangle::Edge edge) const;
	void GetAllPorts(PortVec& ports) const;

	void GetConnectedLinks(LinkVec& links) const;
	void GetConnectedLinks(LinkSet& links) const;
	void GetConnectedJoints(LinkJointVec& joints) const;
	void GetConnectedJoints(LinkJointSet& joints) const;
	void GetConnectedInputJoints(LinkJointVec& joints) const;
	void GetConnectedInputJoints(LinkJointSet& joints) const;
	void GetConnectedOutputJoints(LinkJointVec& joints) const;
	void GetConnectedOutputJoints(LinkJointSet& joints) const;

	void RotatePorts();
	void Move(const MPVector& delta);
	void AdjustGeometry(const MPRectangle& geometry);
	void AdjustLocation(MPVector::Dimension dim, int value);
	void AdjustDimension(MPVector::Axis dim, int width, int height);
	void DisconnectLinks();
	
	class XSC_API BlockMemento : public ComponentMemento
	{
	public:
		BlockMemento(Block* block);
		virtual void Restore() override;

	protected:
		MPRectangle mGeometry;
		InputPortVec mInputPorts;
		OutputPortVec mOutputPorts;
		PortVec mPorts[MPRectangle::NUMBER_OF_EDGES];
	};
	virtual MementoPtr SaveState() const override;
		
	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;
	
	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;

protected:
	Block();
	virtual void OnConstruct(const MPVector& center) = 0;

	InputPort* AddInputPort(MPRectangle::Edge attachSide, const Rtti& portType = XSC_RTTI(InputPort));
	OutputPort* AddOutputPort(MPRectangle::Edge attachSide, const Rtti& portType = XSC_RTTI(OutputPort));
	void RemovePorts(const PortSet& ports, bool completely = true);

	void UpdatePortGeometry();
	void UpdateConnectedLinks();

protected:
	MPRectangle mGeometry;
	InputPortVec mInputPorts;
	OutputPortVec mOutputPorts;
	PortVec mPorts[MPRectangle::NUMBER_OF_EDGES];
};

} // namespace xsc
