#include "hsuiPrecompiled.h"
#include "hsuiParamManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
ParamManager* theParamManager;

void ParamManager::InitParamManager()
{
	theParamManager = new ParamManager;
}

void ParamManager::TermParamManager()
{
	delete theParamManager;
}
// ====================================================================================================================

ParamManager::ParamManager()
{
	theNotiCenter->RegisterListener(static_cast<INotiListener*>(this));
}

ParamManager::~ParamManager()
{
	theNotiCenter->UnregisterListener(static_cast<INotiListener*>(this));
}

void ParamManager::OpenWindow(Component* component, CWnd* owner)
{
	ComponentToWindow::const_iterator iter;
	iter = mWindows.find(component);
	if (iter == mWindows.end())
	{
		auto paramWindow = ParamWindow::CreateDynamicInstance(component);
		if (paramWindow)
		{
			paramWindow->CreateWnd(owner);
		}
	}
	else
	{
		iter->second->BringWindowToTop();
	}
}

void ParamManager::OnNotiChildRemoval(Component*, const ComponentSet& components)
{
	mTempParamWindowVec.clear();
	for (auto cwPair : mWindows)
	{
		for (auto component : components)
		{
			if (component->Contains(cwPair.first))
			{
				mTempParamWindowVec.push_back(cwPair.second);
				break;
			}
		}
	}

	for (auto remove : mTempParamWindowVec)
	{
		remove->DestroyWindow();
	}
}

void ParamManager::OnNotiChildAddition(Component*, const ComponentSet& components)
{
}

void ParamManager::OnNotiPropertyChange(Component* sender, Entity::PropID prop)
{
	if (Component::PROP_NAME == prop)
	{
		auto iter = mWindows.find(sender);
		if (iter != mWindows.end())
		{
			iter->second->Invalidate(FALSE);
		}
	}
}

void ParamManager::RegisterWindow(ParamWindow* paramWindow)
{
	mWindows[paramWindow->GetComponent()] = paramWindow;
}

void ParamManager::UnregisterWindow(ParamWindow* paramWindow)
{
	mWindows.erase(paramWindow->GetComponent());
}

} // namespace hsui
