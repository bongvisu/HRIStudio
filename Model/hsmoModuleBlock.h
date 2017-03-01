#pragma once

#include <hsmoIModuleBlock.h>
#include <hsmoModuleInPort.h>
#include <hsmoModuleOutPort.h>
#include <hsmoModuleLevel.h>
#include <hsmoHriBlock.h>

namespace hsmo {

class HSMO_API ModuleBlock : public HriBlock, public IModuleBlock
{
	XSC_DECLARE_DYNCREATE(ModuleBlock)

public:
	virtual ~ModuleBlock();

	ModuleLevel* GetModuleLevel() const;
	virtual void SetName(const std::wstring& name) override;

	virtual bool IsChild(const xsc::Component* component) const override;
	virtual bool Contains(const xsc::Component* component) const override;
	virtual void GetChildren(xsc::ComponentVec& children, bool deep) const override;
	virtual void GetChildren(xsc::ComponentSet& children, bool deep) const override;
	virtual int GetIndexFor(const xsc::Port* port) const override;

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
	ModuleBlock();
	virtual void OnConstruct(const xsc::MPVector& center) override;
	
	virtual void HandleIOBlockAddition(const ModuleIOBlockSet& ioBlock) override;
	virtual void HandleIOBlockRemoval(const ModuleIOBlockSet& ioBlocks) override;

protected:
	ModuleLevelPtr mLevel;

	// ================================================================================================================
	private: friend Module;
	static void InitModuleBlock();
	static void TermModuleBlock();
	// ================================================================================================================
};

} // namespace hsmo
