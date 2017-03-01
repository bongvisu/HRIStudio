#pragma once

#include <xscOutputPort.h>
#include <hsmoIModuleIOPort.h>

namespace hsmo {

class ModuleOutPort;
typedef xsc::Pointer<ModuleOutPort> ModuleOutPortPtr;
typedef std::list<ModuleOutPort*> ModuleOutPortList;
typedef std::vector<ModuleOutPort*> ModuleOutPortVec;
typedef std::unordered_set<ModuleOutPort*> ModuleOutPortSet;

class ModuleIOBlock;

class HSMO_API ModuleOutPort : public xsc::OutputPort, public IModuleIOPort
{
	XSC_DECLARE_DYNCREATE(ModuleOutPort)

public:
	virtual ~ModuleOutPort();

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
	ModuleOutPort();

protected:
	ModuleIOBlock* mCompanionBlock;

	// ================================================================================================================
	private: friend Module;
	static void InitModuleOutPort();
	static void TermModuleOutPort();
	// ================================================================================================================
};

} // namespace hsmo
