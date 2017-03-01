#pragma once

#include <xscILevel.h>

namespace xsc {

class Level;
typedef Pointer<Level> LevelPtr;
typedef std::list<Level*> LevelList;
typedef std::vector<Level*> LevelVec;
typedef std::unordered_set<Level*> LevelSet;
typedef std::unordered_map<Level*, BlockSet> LevelToBlockSet;
typedef std::unordered_map<Level*, LinkSet> LevelToLinkSet;
typedef std::unordered_map<Level*, LinkJointSet> LevelToJointSet;
typedef std::unordered_map<Level*, LinkSegmentSet> LevelToSegmentSet;
typedef std::unordered_map<Level*, ComponentSet> LevelToComponentSet;

class XSC_API Level
	:
	public Component,
	public ILevel
{
	XSC_DECLARE_DYNAMIC(Level)

public:
	virtual ~Level();
	
	virtual Component* GetLevel() const override;

	const BlockList& GetBlocks() const;
	const LinkList& GetLinks() const;

	virtual void HandlePortRemoval(const PortSet& ports) override;
	virtual void OnChildGeometryChange() override;

	virtual void AddBlock(const std::wstring& rttiName, const MPVector& center);
	virtual void AddLink(OutputPort* source, InputPort* target);
	virtual void ExtendLink(LinkJoint* joint, InputPort* target);
	virtual void ExtendLink(LinkSegment* segment, MPVector point, InputPort* target);
	virtual void InsertComponents(const ComponentSet& components);
	virtual void RemoveComponents(const ComponentSet& components);

	void RotateBlocks(const BlockSet& blocks);
	void MoveComponents(const ComponentSet& components, const MPVector& delta);
	void AlignBlocks(const BlockSet& blocks, MPVector::Direction direction, int value);
	void ModifyBlockGeometry(const BlockSet& blocks, const MPRectangle& geometry);
	void ModifyBlockDimension(const BlockSet& blocks, MPVector::Axis dim, int width, int height);
	void MofifyJointLocation(const LinkJointSet& joints, MPVector::Dimension dim, int value);

	class XSC_API LevelMemento : public ComponentMemento
	{
	public:
		LevelMemento(Level* level);
		virtual void Restore() override;

	protected:
		BlockList mBlocks;
		LinkList mLinks;
	};
	virtual MementoPtr SaveState() const override;

	virtual Component* Clone(ComponentToComponent& remapper) override;
	virtual void Remap(const ComponentToComponent& remapper) override;

	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;

protected:
	Level();

	Block* NewBlock(const std::wstring& rttiName);
	Link* NewLink();

protected:
	BlockList mBlocks;
	LinkList mLinks;
};

} // namespace xsc
