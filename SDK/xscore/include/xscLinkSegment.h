#pragma once

#include <xscLinkElement.h>
#include <xscILinkSegment.h>

namespace xsc {

class LinkSegment;
typedef Pointer<LinkSegment> LinkSegmentPtr;
typedef std::list<LinkSegment*> LinkSegmentList;
typedef std::vector<LinkSegment*> LinkSegmentVec;
typedef std::unordered_set<LinkSegment*> LinkSegmentSet;

class XSC_API LinkSegment
	:
	public LinkElement,
	public ILinkSegment
{
	XSC_DECLARE_DYNCREATE(LinkSegment)

public:
	static const PropID PROP_SOURCE_LOCATION = LinkElement::LAST_PROP;
	static const PropID PROP_DESTINATION_LOCATION = PROP_SOURCE_LOCATION + 1;
	static const PropID LAST_PROP = PROP_DESTINATION_LOCATION + 1;

public:
	LinkSegment();
	virtual ~LinkSegment();

	virtual ConnectableType GetConnectableType() const override;

	virtual Connectable* GetSourceJoint() const override;
	virtual Connectable* GetDestinationJoint() const override;
	virtual const MPVector& GetSourceLocation() const override;
	virtual const MPVector& GetDestinationLocation() const override;

	virtual MPVector::Axis GetAlignment() const override;
	MPVector GetCrossPoint(const MPVector& approximate) const;

	void Move(const MPVector& delta);
	void SynchronizeLocationToJoints();
	void SynchronizeLocationTo(const Connectable* connectable);

	void ConnectSource(Connectable* connectable);
	void ConnectDestination(Connectable* connectable);
	void DisconnectSource();
	void DisconnectDestination();
	void MergeSourceToDestination();
	void MergeDestinationToSource();

	virtual void SetSourceJoint(Connectable* connectable) override;
	virtual void SetDestinationJoint(Connectable* connectable) override;
	virtual void SetSourceLocation(const MPVector& location) override;
	virtual void SetDestinationLocation(const MPVector& location) override;

	virtual void Isolate() override;
	void VerifyForIsolation(ComponentSet& remove);
	virtual void InvalidateUpward(Connectable* originator, ComponentSet& remove) override;
	virtual void InvalidateDownward(Connectable* originator, ComponentSet& remove) override;

	class XSC_API LinkSegmentMemento : public ComponentMemento
	{
	public:
		LinkSegmentMemento(LinkSegment* segment);
		virtual void Restore() override;

	protected:
		MPVector mSourceLocation;
		MPVector mDestinationLocation;
		Connectable* mSourceJoint;
		Connectable* mDestinationJoint;
	};
	virtual MementoPtr SaveState() const override;

	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;

	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;

protected:
	MPVector mSourceLocation;
	MPVector mDestinationLocation;
	Connectable* mSourceJoint;
	Connectable* mDestinationJoint;

public:
	static void Init_LinkSegment();
	static void Term_LinkSegment();
};

} // namespace xsc
