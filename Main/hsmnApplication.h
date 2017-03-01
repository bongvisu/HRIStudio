#pragma once

#include <xscTimer.h>
#include <hsuiIApplication.h>
#include "hsmnResource.h"
#include "hsmnILoopConsumer.h"

namespace hsmn {

class Application
	:
	public CWinAppEx,
	public hsui::IApplication
{
	DECLARE_MESSAGE_MAP()

public:
	Application();

	virtual BOOL InitInstance() override;
	virtual int  ExitInstance() override;
	
	double GetAppTime() const;

	void RegisterLoopConsumer(ILoopConsumer* updateTarget);
	void UnregisterLoopConsumer(ILoopConsumer* updateTarget);

	const CString& GetAppDataDirectory() const;

	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileClose();
	afx_msg void OnFileSave();

	afx_msg void OnAppAbout();
	afx_msg void OnHelp();

	virtual HINSTANCE GetApplicationInstanceHandle() const override;
	virtual CString GetApplicationTitle() const override;
	virtual UINT GetApplicationIconActive() const override;
	virtual UINT GetApplicationIconInactive() const override;
	virtual bool IsMdiActive() const override;
	virtual void UpdateLoopConsumers() override;

	virtual BOOL OnIdle(LONG lCount) override;

protected:
	virtual void PreLoadState() override;
	virtual void LoadCustomState() override;
	virtual void SaveCustomState() override;
	

	void LoadStateInfo();
	void SaveStateInfo();

private:
	CString mAppDataDirectory;

	xsc::Timer mTimer;
	std::unordered_set<ILoopConsumer*> mLoopConsumers;
};

extern Application theApp;

} // namespace hsmn
