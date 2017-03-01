#pragma once

#include <hsmoModuleIOBlock.h>

namespace hsmo {

class ModuleOutBlock;
typedef xsc::Pointer<ModuleOutBlock> ModuleOutBlockPtr;
typedef std::list<ModuleOutBlock*> ModuleOutBlockList;
typedef std::vector<ModuleOutBlock*> ModuleOutBlockVec;
typedef std::unordered_set<ModuleOutBlock*> ModuleOutBlockSet;

class HSMO_API ModuleOutBlock : public ModuleIOBlock
{
	XSC_DECLARE_DYNCREATE(ModuleOutBlock)

public:
	virtual ~ModuleOutBlock();

	virtual xsc::PortType GetCompatiblePortType() const override;

protected:
	ModuleOutBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;

	// ================================================================================================================
	private: friend Module;
	static void InitModuleOutBlock();
	static void TermModuleOutBlock();
	// ================================================================================================================
};

} // namespace hsmo
