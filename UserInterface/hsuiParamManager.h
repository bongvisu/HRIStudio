#pragma once

#include <xscComponent.h>
#include <xscNotification.h>
#include <hsuiParamWindow.h>

namespace hsui {

class HSUI_API ParamManager : public xsc::INotiListener
{
protected:
	ParamManager();
	~ParamManager();

public:
	void OpenWindow(xsc::Component* component, CWnd* owner);

	virtual void OnNotiChildAddition(xsc::Component* sender, const xsc::ComponentSet& components) override;
	virtual void OnNotiChildRemoval(xsc::Component* sender, const xsc::ComponentSet& components) override;
	virtual void OnNotiPropertyChange(xsc::Component* sender, xsc::Entity::PropID prop) override;

public:
	void RegisterWindow(ParamWindow* paramWindow);
	void UnregisterWindow(ParamWindow* paramWindow);

protected:
	typedef std::unordered_map<const xsc::Component*, ParamWindow*> ComponentToWindow;
	ComponentToWindow mWindows;

	std::vector<ParamWindow*> mTempParamWindowVec;

	// ================================================================================================================
	private: friend Module;
	static void InitParamManager();
	static void TermParamManager();
	// ================================================================================================================
};

extern HSUI_API ParamManager* theParamManager;

} // namespace hsui
