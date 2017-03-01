#pragma once

#include <hsmoModuleIOBlock.h>

namespace hsmo {

class ModuleInBlock;
typedef xsc::Pointer<ModuleInBlock> ModuleInBlockPtr;
typedef std::list<ModuleInBlock*> ModuleInBlockList;
typedef std::vector<ModuleInBlock*> ModuleInBlockVec;
typedef std::unordered_set<ModuleInBlock*> ModuleInBlockSet;

class HSMO_API ModuleInBlock : public ModuleIOBlock
{
	XSC_DECLARE_DYNCREATE(ModuleInBlock)

public:
	virtual ~ModuleInBlock();

	virtual xsc::PortType GetCompatiblePortType() const override;

protected:
	ModuleInBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

	// ================================================================================================================
	private: friend Module;
	static void InitModuleInBlock();
	static void TermModuleInBlock();
	// ================================================================================================================
};

} // namespace hsmo
