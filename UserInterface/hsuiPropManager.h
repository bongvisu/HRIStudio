#pragma once

#include <xscNotification.h>
#include <xscComponent.h>
#include <hsuiPropGridCtrl.h>
#include <hsuiPropHandler.h>

namespace hsui {

class HSUI_API PropManager : public xsc::INotiListener
{
public:
	PropGridCtrl* CreateView(CWnd* parent);
	void DestroyView();
	
	PropGridCtrl& GetView();

	void SetTargets(xsc::ComponentSet&& targets);
	const xsc::ComponentSet& GetTargets() const;
	int GetNumTargets() const;

	template <typename T>
	const T& GetTargets() const
	{
		return reinterpret_cast<const T&>(mTargets);
	}

protected:
	PropManager();
	~PropManager();

	virtual void OnNotiChildAddition(xsc::Component*, const xsc::ComponentSet&) override;
	virtual void OnNotiChildRemoval(xsc::Component*, const xsc::ComponentSet&) override;
	virtual void OnNotiPropertyChange(xsc::Component*, xsc::Entity::PropID) override;

	void Update();

protected:
	std::unordered_map<const wchar_t*, PropHandler*> mPropHandlers;
	PropHandler* mCurrentHandler;

	PropGridCtrl mView;
	xsc::ComponentSet mTargets;

	xsc::ComponentVec mTempComponentVec;

	// ================================================================================================================
	private: friend Module;
	static void InitPropManager();
	static void TermPropManager();
	// ================================================================================================================
};

extern HSUI_API PropManager* thePropManager;

} // namespace hsui
