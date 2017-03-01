#pragma once

#include <hsmoModel.h>

namespace hsmo {

class HSMO_API IModelManagerObserver
{
public:
	virtual void OnCodeTransferStarting() = 0;
	virtual void OnCodeTransferCompleted() = 0;

	virtual void OnMonitorConnected() = 0;
	virtual void OnMonitorDisconnected() = 0;
	virtual void OnMonitorStateUpdated() = 0;

protected:
	virtual ~IModelManagerObserver();
};

class HSMO_API ModelManager : public xsc::Component
{
public:
	static const PropID PROP_ACTIVE_MODEL = xsc::Component::LAST_PROP;
	static const PropID LAST_PROP = PROP_ACTIVE_MODEL + 1;

public:
	int GetNumModels() const;
	const ModelList& GetModels() const;
	
	void NewModel(const std::wstring& filepath);
	void OpenModel(const std::wstring& filepath);
	void Save();

	void CloseActiveModel();
	void CloseModel(Model* model);

	void SetActiveModel(Model* model);
	Model* GetActiveModel() const;

	bool IsClipboardAvailable() const;

	// ----------------------------------------------------------------------------------------------------------------
	void AddObserver(IModelManagerObserver* observer);
	void RemoveObserver(IModelManagerObserver* observer);

	void SetPlatformIP(const std::wstring& ip);
	void SetPlatformPort(int port);
	void SetPlatformID(const std::wstring& id);
	void SetPlatformPassword(const std::wstring& password);
	void SetPlatformDirectory(const std::wstring& directory);
	const std::wstring& GetPlatformIP() const;
					int	GetPlatformPort() const;
	const std::wstring& GetPlatformID() const;
	const std::wstring& GetPlatformPassword() const;
	const std::wstring& GetPlatformDirectory() const;
	
	void DownloadCode(const std::wstring& code);
	int GetDownloadProgress() const;
	
	void StartMonitoring();
	void StopMonitoring();
	// ----------------------------------------------------------------------------------------------------------------

protected:
	ModelManager();
	~ModelManager();

	static LRESULT CALLBACK MessagingWndProcEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MessagingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CodeTransferThread();

	static LRESULT CALLBACK MonWndProcEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MonWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void MonitorThread();

protected:
	ModelList mModels;
	Model* mActiveModel;

	friend class CutCommand;
	friend class CopyCommand;
	friend class PasteCommand;
	xsc::ComponentPtrVec mClipboard;

	HWND mMessagingWnd;
	std::set<IModelManagerObserver*> mObservers;
	CRITICAL_SECTION mCriticalSection;

	std::string mCode;
	HANDLE mCTEvents[2]; /* 0:quit, 1:do transfer */
	std::thread mCTThread;
	std::atomic<int> mCTProgress;
	bool mCTDownloading;

	std::thread mMonThread;
	std::atomic<HWND> mMonWnd;

	// ----------------------------------------------------------------------------------------------------------------
	std::wstring mPlatformIP;
	int			 mPlatformPort;
	std::wstring mPlatformID;
	std::wstring mPlatformPassword;
	std::wstring mPlatformDirectory;
	// ----------------------------------------------------------------------------------------------------------------

	// ================================================================================================================
	private: friend Module;
	static void InitModelManager();
	static void TermModelManager();
	// ================================================================================================================
};

extern HSMO_API ModelManager* theModelManager;

} // namespace hsmo
