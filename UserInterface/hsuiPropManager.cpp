#include "hsuiPrecompiled.h"
#include "hsuiPropManager.h"
#include "hsuiVisualManager.h"

#include "hsuiPHObject.h"
#include "hsuiPHComponent.h"
#include "hsuiPHBlock.h"
#include "hsuiPHLinkJoint.h"
#include "hsuiPHLinkSegment.h"
#include "hsuiPHModel.h"

#include <xscBlock.h>
#include <xscLinkJoint.h>
#include <xscLinkSegment.h>
#include <hsmoModel.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
PropManager* thePropManager;

void PropManager::InitPropManager()
{
	thePropManager = new PropManager;
}

void PropManager::TermPropManager()
{
	delete thePropManager;
}
// ====================================================================================================================

PropManager::PropManager()
{
	mPropHandlers[XSC_RTTI(Object).GetName()] = PHObject::GetInstance();
	mPropHandlers[XSC_RTTI(Component).GetName()] = PHComponent::GetInstance();
	mPropHandlers[XSC_RTTI(Block).GetName()] = PHBlock::GetInstance();
	mPropHandlers[XSC_RTTI(LinkJoint).GetName()] = PHLinkJoint::GetInstance();
	mPropHandlers[XSC_RTTI(LinkSegment).GetName()] = PHLinkSegment::GetInstance();
	mPropHandlers[XSC_RTTI(Model).GetName()] = PHModel::GetInstance();

	mCurrentHandler = PHObject::GetInstance();

	theNotiCenter->RegisterListener(static_cast<INotiListener*>(this));
}

PropManager::~PropManager()
{
	theNotiCenter->UnregisterListener(static_cast<INotiListener*>(this));
}

PropGridCtrl* PropManager::CreateView(CWnd* parent)
{
	mView.Create(WS_CHILD | WS_VISIBLE, { 0, 0, 8, 8 }, parent, AFX_IDW_PANE_FIRST);
	mView.SetFont(theVisualManager->GetCommonFontR(), FALSE);
	return &mView;
}

void PropManager::DestroyView()
{
	mView.DestroyWindow();
}

PropGridCtrl& PropManager::GetView()
{
	return mView;
}

void PropManager::OnNotiChildAddition(Component*, const ComponentSet&)
{
}

void PropManager::OnNotiChildRemoval(Component*, const ComponentSet& components)
{
	auto numTargets = static_cast<int>(mTargets.size());
	if (numTargets == 0)
	{
		return;
	}

	mTempComponentVec.clear();
	for (auto target : mTargets)
	{
		for (auto component : components)
		{
			if (component->Contains(target))
			{
				mTempComponentVec.push_back(target);
				break;
			}
		}
	}
	for (auto remove : mTempComponentVec)
	{
		mTargets.erase(remove);
	}

	if (mTargets.size() != numTargets)
	{
		Update();
	}
}

void PropManager::OnNotiPropertyChange(Component* component, Entity::PropID prop)
{
	if (mTargets.find(component) != mTargets.end())
	{
		mCurrentHandler->ReflectChange(prop);
	}
}

void PropManager::Update()
{
	Rtti largestCommon(XSC_RTTI(Object));

	if (mTargets.size() != 0)
	{
		ComponentSet::const_iterator iter = mTargets.begin();
		largestCommon = (*iter)->GetRtti();

		iter++;
		for (/**/; iter != mTargets.end(); ++iter)
		{
			while (false == (*iter)->IsDerivedFrom(largestCommon))
			{
				largestCommon = *largestCommon.GetBase();
			}
		}
	}

	unordered_map<const wchar_t*, PropHandler*>::iterator iter;
	iter = mPropHandlers.find(largestCommon.GetName());
	while (iter == mPropHandlers.end())
	{
		largestCommon = *largestCommon.GetBase();
		iter = mPropHandlers.find(largestCommon.GetName());
	}

	mCurrentHandler = iter->second;

	mView.LockWindowUpdate();
	mCurrentHandler->Setup();
	mView.UnlockWindowUpdate();
}

void PropManager::SetTargets(ComponentSet&& targets)
{
	mTargets = std::move(targets);
	Update();
}

const ComponentSet& PropManager::GetTargets() const
{
	return mTargets;
}

int PropManager::GetNumTargets() const
{
	return (int)mTargets.size();
}

} // namespace hsui
