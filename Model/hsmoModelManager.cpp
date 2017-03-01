#include "hsmoPrecompiled.h"
#include "hsmoModelManager.h"
#include "hsmoLogMediator.h"
#include <xscPathUtil.h>
#include <xscStringUtil.h>
#include <CkSFtp.h>
#include <CkByteData.h>
#include <CkSettings.h>

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
ModelManager* theModelManager;

void ModelManager::InitModelManager()
{
	theModelManager = new ModelManager;
}

void ModelManager::TermModelManager()
{
	delete theModelManager;
}
// ====================================================================================================================

static const wchar_t* MESSAGING_WND_CLASS = L"hsmo.ModelManager.MessagingWnd";
static const wchar_t* MONITOR_WND_CLASS = L"hsmo.ModelManager.MonitorWnd";

void ModelManager::SetPlatformIP(const wstring& ip)
{
	EnterCriticalSection(&mCriticalSection);
	mPlatformIP = ip;
	LeaveCriticalSection(&mCriticalSection);
}

void ModelManager::SetPlatformPort(int port)
{
	EnterCriticalSection(&mCriticalSection);
	mPlatformPort = port;
	LeaveCriticalSection(&mCriticalSection);
}

void ModelManager::SetPlatformID(const wstring& id)
{
	EnterCriticalSection(&mCriticalSection);
	mPlatformID = id;
	LeaveCriticalSection(&mCriticalSection);
}

void ModelManager::SetPlatformPassword(const wstring& password)
{
	EnterCriticalSection(&mCriticalSection);
	mPlatformPassword = password;
	LeaveCriticalSection(&mCriticalSection);
}

void ModelManager::SetPlatformDirectory(const wstring& directory)
{
	EnterCriticalSection(&mCriticalSection);
	mPlatformDirectory = directory;
	LeaveCriticalSection(&mCriticalSection);
}

const wstring& ModelManager::GetPlatformIP() const
{
	return mPlatformIP;
}

int ModelManager::GetPlatformPort() const
{
	return mPlatformPort;
}

const wstring& ModelManager::GetPlatformID() const
{
	return mPlatformID;
}

const wstring& ModelManager::GetPlatformPassword() const
{
	return mPlatformPassword;
}

const wstring& ModelManager::GetPlatformDirectory() const
{
	return mPlatformDirectory;
}

IModelManagerObserver::~IModelManagerObserver()
{
}

void ModelManager::AddObserver(IModelManagerObserver* observer)
{
	mObservers.insert(observer);
}

void ModelManager::RemoveObserver(IModelManagerObserver* observer)
{
	mObservers.erase(observer);
}

int ModelManager::GetDownloadProgress() const
{
	return mCTProgress;
}

void ModelManager::DownloadCode(const wstring& code)
{
	if (mCTDownloading)
	{
		LogItem logItem;
		logItem.senderID = mID;
		logItem.senderName = mName;
		logItem.message = L"Code download in progress. Wait until the job is completed.";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}
	mCTDownloading = true;

	StringUtil::ToMultiByte(code, mCode);
	for (auto observer : mObservers)
	{
		observer->OnCodeTransferStarting();
	}
	SetEvent(mCTEvents[1]);
}

static const UINT_PTR WU_DOWNLOAD_COMPLETE = WM_APP + 1;
static const UINT_PTR WU_MONITOR_CONNECTED = WM_APP + 2;
static const UINT_PTR WU_MONITOR_DISCONNECTED = WM_APP + 3;
static const UINT_PTR WU_MONITOR_UPDATED = WM_APP + 4;

LRESULT CALLBACK ModelManager::MessagingWndProcEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return theModelManager->MessagingWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ModelManager::MessagingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WU_DOWNLOAD_COMPLETE)
	{
		mCTProgress = 0;
		mCTDownloading = false;

		for (auto observer : mObservers)
		{
			observer->OnCodeTransferCompleted();
		}
		
		return 0;
	}
	else if (message == WU_MONITOR_CONNECTED)
	{
		return 0;
	}
	else if (message == WU_MONITOR_DISCONNECTED)
	{
		return 0;
	}
	else if (message == WU_MONITOR_UPDATED)
	{
		return 0;
	}

	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void ModelManager::CodeTransferThread()
{
	string path;
	string completionMessage;
	while (true)
	{
		auto result = WaitForMultipleObjects(2, mCTEvents, FALSE, INFINITE);
		if (result == WAIT_OBJECT_0)
		{
			return;
		}
		
		string host, id, password, directory; int port;
		EnterCriticalSection(&mCriticalSection);
		StringUtil::ToMultiByte(mPlatformIP, host);
		port = mPlatformPort;
		StringUtil::ToMultiByte(mPlatformID, id);
		StringUtil::ToMultiByte(mPlatformPassword, password);
		StringUtil::ToMultiByte(mPlatformDirectory, directory);
		LeaveCriticalSection(&mCriticalSection);

		CkSFtp sftp;
		bool success = sftp.UnlockComponent("DONGHO.SS10317_TGh5hPdc9UlN");
		if (false == success)
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			logItem.message = L"Chilkat library unlocking failed. Check out the license.";
			theLogMediator->TransferLogItem(std::move(logItem));
			goto MM_TRANSFER_COMPLETION;
		}

		sftp.put_ConnectTimeoutMs(5000);
		sftp.put_IdleTimeoutMs(10000);
		success = sftp.Connect(host.c_str(), port);
		if (false == success)
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			logItem.message = L"Connection to server failed. Check out the environment.";
			theLogMediator->TransferLogItem(std::move(logItem));
			goto MM_TRANSFER_COMPLETION;
		}

		success = sftp.AuthenticatePw(id.c_str(), password.c_str());
		if (false == success)
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			logItem.message = L"Authentication failed. Check out the account.";
			theLogMediator->TransferLogItem(std::move(logItem));
			goto MM_TRANSFER_COMPLETION;
		}

		success = sftp.InitializeSftp();
		if (false == success)
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			logItem.message = L"SFTP initialization failed. Check out the environment.";
			theLogMediator->TransferLogItem(std::move(logItem));
			goto MM_TRANSFER_COMPLETION;
		}

		path = directory + "/brain.rive";
		auto handle = sftp.openFile(path.c_str(), "writeOnly", "createTruncate");
		if (nullptr == handle)
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			logItem.message = L"Remote file open failed. Check out the environment.";
			theLogMediator->TransferLogItem(std::move(logItem));
			goto MM_TRANSFER_COMPLETION;
		}

		int len = static_cast<int>(mCode.length());
		int chunk = len / 10;
		int start = 0;
		for (int i = 0; i < 10; ++i)
		{
			if (i == 9)
			{
				chunk = len - start;
			}
			
			if (chunk)
			{
				CkByteData byteData;
				byteData.appendStr(mCode.substr(start, chunk).c_str());
				start += chunk;

				success = sftp.WriteFileBytes(handle, byteData);
				if (false == success)
				{
					sftp.CloseHandle(handle);
					goto MM_TRANSFER_COMPLETION;
				}
			}

			Sleep(30);
			mCTProgress += 10;
		}
		sftp.CloseHandle(handle);
		Sleep(300);
		{
			LogItem logItem;
			logItem.senderID = mID;
			logItem.senderName = mName;
			completionMessage = "Code transfer complete. [";
			completionMessage += path + "]";
			StringUtil::ToWide(completionMessage, logItem.message);
			theLogMediator->TransferLogItem(std::move(logItem));
		}

MM_TRANSFER_COMPLETION:
		PostMessageW(mMessagingWnd, WU_DOWNLOAD_COMPLETE, 0, 0);
	}
}

static const UINT_PTR WU_CONNECT = WM_APP + 1;
static const UINT_PTR WU_DISCONNECT = WM_APP + 2;

LRESULT CALLBACK ModelManager::MonWndProcEntry(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return theModelManager->MonWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ModelManager::MonWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WU_CONNECT)
	{
		return 0;
	}
	else if (message == WU_DISCONNECT)
	{
		return 0;
	}

	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void ModelManager::MonitorThread()
{
	mMonWnd = CreateWindowExW(0, MONITOR_WND_CLASS, nullptr,
		WS_CHILD, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);

	MSG msg;
	while (GetMessageW(&msg, mMonWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	DestroyWindow(mMonWnd);
}

void ModelManager::StartMonitoring()
{
}

void ModelManager::StopMonitoring()
{
}

ModelManager::ModelManager()
{
	WSADATA wsadat;
	WSAStartup(MAKEWORD(2, 2), &wsadat);
	InitializeCriticalSection(&mCriticalSection);

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = MessagingWndProcEntry;
	wc.lpszClassName = MESSAGING_WND_CLASS;
	RegisterClassW(&wc);

	mMessagingWnd = CreateWindowExW(0, MESSAGING_WND_CLASS, nullptr,
		WS_CHILD, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);

	mName = L"The Model Manager";
	mActiveModel = nullptr;

	mPlatformIP = L"localhost";
	mPlatformPort = 22;
	mPlatformID = L"ubuntu";
	mPlatformPassword = L"ubuntu";
	mPlatformDirectory = L"catkin_ws/src/mhri_ros/dialog_scripts/scripts/raspberry";

	mCTProgress = 0;
	mCTDownloading = false;
	mCTEvents[0] = CreateEventW(nullptr, TRUE, FALSE, nullptr);
	mCTEvents[1] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
	mCTThread = std::thread(&ModelManager::CodeTransferThread, this);

	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = MonWndProcEntry;
	wc.lpszClassName = MONITOR_WND_CLASS;
	RegisterClassW(&wc);
	mMonWnd = nullptr;
	mMonThread = std::thread(&ModelManager::MonitorThread, this);
	while (true)
	{
		HWND isReady = mMonWnd;
		if (isReady) { break;    }
		else		 { Sleep(0); }
	}
}

ModelManager::~ModelManager()
{
	PostMessageW(mMonWnd, WM_QUIT, 0, 0);
	mMonThread.join();
	
	SetEvent(mCTEvents[0]);
	mCTThread.join();
	CloseHandle(mCTEvents[0]);
	CloseHandle(mCTEvents[1]);

	DestroyWindow(mMessagingWnd);

	DeleteCriticalSection(&mCriticalSection);
	CkSettings::cleanupMemory();
	WSACleanup();
}

int ModelManager::GetNumModels() const
{
	return static_cast<int>(mModels.size());
}

const ModelList& ModelManager::GetModels() const
{
	return mModels;
}

void ModelManager::SetActiveModel(Model* model)
{
	if (mActiveModel == model)
	{
		return;
	}

	mActiveModel = model;
	NotifyPropertyChange(PROP_ACTIVE_MODEL);
}

Model* ModelManager::GetActiveModel() const
{
	return mActiveModel;
}

void ModelManager::NewModel(const wstring& filepath)
{
	auto model(new Model(filepath));
	model->SetParent(this);

	mModels.push_back(model);
	mChildren.insert(model);

	ComponentSet newComponents;
	newComponents.insert(model);
	NotifyChildAddition(newComponents);

	mActiveModel = model;
	NotifyPropertyChange(PROP_ACTIVE_MODEL);
}

void ModelManager::OpenModel(const wstring& filepath)
{
	wstring directory, name;
	PathUtil::SplitToDirectoryAndName(filepath, directory, name);

	auto model(new Model(filepath));
	model->SetParent(this);
	if (false == model->Load())
	{
		LogItem logItem;
		logItem.senderName = mName;
		logItem.senderID = mID;
		logItem.message = L"Model load failed. Check out the path <";
		logItem.message += filepath + L">.";
		theLogMediator->TransferLogItem(std::move(logItem));

		delete model;
		return; // model load failed.
	}

	mModels.push_back(model);
	mChildren.insert(model);

	ComponentSet newComponents;
	newComponents.insert(model);
	NotifyChildAddition(newComponents);

	mActiveModel = model;
	NotifyPropertyChange(PROP_ACTIVE_MODEL);
}

void ModelManager::Save()
{
	for (auto model : mModels)
	{
		if (false == model->Save())
		{
			LogItem logItem;
			logItem.senderName = mName;
			logItem.senderID = mID;
			logItem.message = wstring(L"Model load failed. Check out the path <");
			logItem.message += model->GetFilePath() + L">.";
			theLogMediator->TransferLogItem(std::move(logItem));
		}
	}
}

void ModelManager::CloseActiveModel()
{
	if (mActiveModel)
	{
		CloseModel(mActiveModel);
	}
}

void ModelManager::CloseModel(Model* model)
{
	ModelList::const_iterator iter;
	for (iter = mModels.begin(); iter != mModels.end(); ++iter)
	{
		if ((*iter) == model)
		{
			break;
		}
	}
	assert(iter != mModels.end());

	ComponentSet components;
	components.insert(model);
	NotifyChildRemoval(components);
	
	mModels.erase(iter);
	if (model == mActiveModel)
	{
		if (mModels.size())
		{
			mActiveModel = *mModels.rbegin();
		}
		else
		{
			mActiveModel = nullptr;
		}
		NotifyPropertyChange(PROP_ACTIVE_MODEL);
	}
	mChildren.erase(model);
}

bool ModelManager::IsClipboardAvailable() const
{
	return (mClipboard.size() != 0);
}

} // namespace hsmo
