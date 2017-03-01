#include "hsmoPrecompiled.h"
#include "hsmoCopyCommand.h"
#include "hsmoModelManager.h"
#include "hsmoModuleIOBlock.h"
#include "hsmoLogMediator.h"

using namespace std;
using namespace xsc;

namespace hsmo {

CopyCommand::CopyCommand(Level* level, ComponentSet&& components)
{
	mLevel = level;
	mComponents = std::move(components);
}

CopyCommand::~CopyCommand()
{
}

bool CopyCommand::Exec()
{
	ComponentSet targets;
	targets.reserve(mComponents.size());

	LinkSet links;
	for (auto component : mComponents)
	{
		if (component->IsDerivedFrom(XSC_RTTI(LinkElement)))
		{
			assert(component->GetParent()->IsDerivedFrom(XSC_RTTI(Link)));
			Link* link = static_cast<Link*>(component->GetParent());
			if (links.find(link) == links.end())
			{
				links.insert(link);
				targets.insert(link);
			}
		}
		else if (component->IsDerivedFrom(XSC_RTTI(ModuleIOBlock)))
		{
			LogItem logItem;
			logItem.senderName = mLevel->GetName();
			logItem.senderID = mLevel->GetID();
			swprintf_s(Component::msGenericTextBuf, Component::GENERIC_TEXT_BUF_SIZE,
				L"Module I/O block <%s> are not copiable. So excluded.", component->GetName().c_str());
			logItem.message = Component::msGenericTextBuf;
			theLogMediator->TransferLogItem(std::move(logItem));
		}
		else
		{
			assert(false == component->IsDerivedFrom(XSC_RTTI(Port)));
			assert(false == component->IsDerivedFrom(XSC_RTTI(Link)));
			targets.insert(component);
		}
	}

	if (targets.size() == 0)
	{
		return false;
	}

	ComponentPtrVec clonedComponents;
	clonedComponents.reserve(targets.size());
	ComponentToComponent remapper;
	for (auto target : targets)
	{
		clonedComponents.push_back(target->Clone(remapper));
	}
	for (auto& cloned : clonedComponents)
	{
		cloned->Remap(remapper);
	}

	theModelManager->mClipboard.clear();
	theModelManager->mClipboard.reserve(clonedComponents.size());
	for (auto& cloned : clonedComponents)
	{
		if (cloned->IsDerivedFrom(XSC_RTTI(Link)))
		{
			Link* link = static_cast<Link*>(cloned.Get());
			if (link->PruneIsolatedElements(false))
			{
				continue;
			}
			theModelManager->mClipboard.push_back(cloned);
		}
		else
		{
			theModelManager->mClipboard.push_back(cloned);
		}
	}

	mComponents = std::move(ComponentSet());
	return false;
}

} // namespace hsmo
