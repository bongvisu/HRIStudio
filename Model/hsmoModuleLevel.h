#pragma once

#include <xscLevel.h>
#include <hsmoModuleInBlock.h>
#include <hsmoModuleOutBlock.h>

namespace hsmo {

class ModuleLevel;
typedef xsc::Pointer<ModuleLevel> ModuleLevelPtr;
typedef std::list<ModuleLevel*> ModuleLevelList;
typedef std::vector<ModuleLevel*> ModuleLevelVec;
typedef std::unordered_set<ModuleLevel*> ModuleLevelSet;

class HSMO_API ModuleLevel : public xsc::Level
{
	XSC_DECLARE_DYNCREATE(ModuleLevel)
	
public:
	virtual ~ModuleLevel();

	virtual void GetHierarchicalPath(std::wstring& path) const override;

	virtual void AddBlock(const std::wstring& rttiName, const xsc::MPVector& center);
	virtual void RemoveComponents(const xsc::ComponentSet& components);

protected:
	friend class ModuleBlock;
	ModuleLevel();

	// ================================================================================================================
	private: friend Module;
	static void InitModuleLevel();
	static void TermModuleLevel();
	// ================================================================================================================
};

} // namespace hsmo
