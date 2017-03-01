#pragma once

#include <xscLinkJoint.h>
#include <xscLinkSegment.h>
#include <xscInputPort.h>
#include <xscOutputPort.h>

namespace xsc {

class Link;
typedef Pointer<Link> LinkPtr;
typedef std::list<Link*> LinkList;
typedef std::vector<Link*> LinkVec;
typedef std::unordered_set<Link*> LinkSet;
typedef std::unordered_map<Link*, ComponentSet> LinkToComponentSet;
typedef std::unordered_map<Link*, LinkElementSet> LinkToElementSet;
typedef std::unordered_map<Link*, LinkJointSet> LinkToJointSet;
typedef std::unordered_map<Link*, LinkSegmentSet> LinkToSegmentSet;
typedef std::vector<std::pair<Port*, LinkJoint*>> PortJointPairVec;

enum LinkLineWidth
{
	LINK_LINE_WIDTH_THIN = 0,
	LINK_LINE_WIDTH_THICK,
	NUMBER_OF_LINK_LINE_WIDTHS
};

enum LinkLineStyle
{
	LINK_LINE_STYLE_SOLID = 0,
	LINK_LINE_STYLE_DASHED,
	NUMBER_OF_LINK_LINE_STYLES
};

enum LinkLineColor
{
	LINK_LINE_COLOR_BLACK = 0,
	LINK_LINE_COLOR_TEAL,
	LINK_LINE_COLOR_PURPLE,
	NUMBER_OF_LINK_LINE_COLORS
};

class XSC_API Link : public Component
{
	XSC_DECLARE_DYNCREATE(Link)

public:
	static void GetOptimalPathFor(MPVector source, MPVector target, std::vector<MPVector>& path);

	static const PropID PROP_LINE_WIDTH = Component::LAST_PROP;
	static const PropID PROP_LINE_STYLE = PROP_LINE_WIDTH + 1;
	static const PropID PROP_LINE_COLOR = PROP_LINE_STYLE + 1;
	static const PropID LAST_PROP = PROP_LINE_COLOR + 1;

public:
	void Construct(OutputPort* source, InputPort* target);
	virtual ~Link();

	const LinkJointSet& GetJoints() const;
	const LinkSegmentSet& GetSegments() const;
	OutputPort* GetSourcePort() const;
	InputPort* GetDestinationPort() const;
	LinkJoint* GetSourceJoint(const ComponentSet& exclusionSet) const;
	LinkJoint* GetDestinationJoint(const ComponentSet& exclusionSet) const;

	void SetLineWidth(LinkLineWidth lineWidth);
	LinkLineWidth GetLineWidth() const;

	void SetLineStyle(LinkLineStyle lineStyle);
	LinkLineStyle GetLineStyle() const;

	void SetLineColor(LinkLineColor lineColor);
	LinkLineColor GetLineColor() const;

	void Extend(LinkJoint* joint, InputPort* target);
	void Extend(LinkSegment* segment, MPVector point, InputPort* target);

	bool PruneIsolatedElements(bool notify);
	void RespondToBlockGeometryChange(const PortJointPairVec& portJointPairs);
	void MoveComponents(const ComponentSet& components, const MPVector& delta);
	void RelocateJoints(const LinkJointSet& joints, MPVector::Dimension dim, int value);

	void DisconnectAnchors();

	class XSC_API LinkMemento : public ComponentMemento
	{
	public:
		LinkMemento(Link* link);
		virtual void Restore() override;

	protected:
		LinkJointSet mJoints;
		LinkSegmentSet mSegments;
	};
	virtual MementoPtr SaveState() const override;

	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;

	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;

protected:
	Link();

	LinkJoint* NewJoint(bool complete);
	LinkSegment* NewSegment(bool complete);

protected:
	LinkLineWidth mLineWidth;
	LinkLineStyle mLineStyle;
	LinkLineColor mLineColor;

	LinkJointSet mJoints;
	LinkSegmentSet mSegments;

protected:
	void PrepareRearrangement(const LinkJointSet&, const LinkSegmentSet&);
	void DoRearrangement(const LinkJointSet&, const LinkSegmentSet&, bool notify);
	void HandleAnchor(LinkJoint*, const LinkJointSet&, const LinkSegmentSet&);
	void HandleIntermediate(LinkJoint*, const LinkJointSet&, const LinkSegmentSet&);

	class Ledger
	{
	public:
		Ledger();
		void Clear();

		bool IsNewlyCreated(LinkElement* element) const;
		bool IsExistingOne(LinkElement* element) const;

		void EnlistNewComponent(LinkElement* element);
		void EnlistCheckUpJoint(LinkJoint* joint);

		void ReleaseEntry(LinkElement* element);
		LinkJoint* PopCheckUpJoint();

		const ComponentSet& GetNewlyCreated() const;
		const ComponentSet& GetToBeRemoved() const;

	protected:
		ComponentSet mNewlyCreated;
		ComponentSet mToBeRemoved;
		ComponentSet mCheckUpJoints;
	};
	Ledger mLedger;
	ConnectableVec mConnectables;

public:
	static void Init_Link();
	static void Term_Link();
};

} // namespace xsc
