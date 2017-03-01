#pragma once

#include <hsmoHriBlock.h>

namespace hsmo {

class ModuleIOBlock;
typedef xsc::Pointer<ModuleIOBlock> ModuleIOBlockPtr;
typedef std::list<ModuleIOBlock*> ModuleIOBlockList;
typedef std::vector<ModuleIOBlock*> ModuleIOBlockVec;
typedef std::unordered_set<ModuleIOBlock*> ModuleIOBlockSet;

class IModuleBlock;

class HSMO_API ModuleIOBlock : public HriBlock
{
	XSC_DECLARE_DYNAMIC(ModuleIOBlock)

public:
	virtual ~ModuleIOBlock();

	virtual xsc::PortType GetCompatiblePortType() const = 0;

	void SetCompanionPort(xsc::Port* companionPort);
	xsc::Port* GetCompanionPort() const;
	int GetIndex() const;

	// Cloning --------------------------------------------------------------------------------------------------------
	virtual xsc::Component* Clone(xsc::ComponentToComponent& remapper) override;
	virtual void Remap(const xsc::ComponentToComponent& remapper) override;
	// ----------------------------------------------------------------------------------------------------------------

	// Stream ---------------------------------------------------------------------------------------------------------
	virtual bool Serialize(Stream& stream) override;
	virtual bool Deserialize(Stream& stream) override;
	virtual bool DoPostloadProcessing(Stream& stream) override;
	// ----------------------------------------------------------------------------------------------------------------

protected:
	ModuleIOBlock();

protected:
	xsc::Port* mCompanionPort;
	mutable IModuleBlock* mModuleBlock;
};

} // namespace hsmo
