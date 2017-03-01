#pragma once

#include <xscInputPort.h>
#include <hsmoIModuleIOPort.h>

namespace hsmo {

class ModuleInPort;
typedef xsc::Pointer<ModuleInPort> ModuleInPortPtr;
typedef std::list<ModuleInPort*> ModuleInPortList;
typedef std::vector<ModuleInPort*> ModuleInPortVec;
typedef std::unordered_set<ModuleInPort*> ModuleInPortSet;

class ModuleIOBlock;

class HSMO_API ModuleInPort : public xsc::InputPort, public IModuleIOPort
{
	XSC_DECLARE_DYNCREATE(ModuleInPort)
	
public:
	virtual ~ModuleInPort();

	virtual void SetComapionBlock(ModuleIOBlock* companionBlock) override;
	virtual ModuleIOBlock* GetCompanionBlock() const override;

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
	ModuleInPort();

protected:
	ModuleIOBlock* mCompanionBlock;

	// ================================================================================================================
	private: friend Module;
	static void InitModuleInPort();
	static void TermModuleInPort();
	// ================================================================================================================
};

} // namespace hsmo
