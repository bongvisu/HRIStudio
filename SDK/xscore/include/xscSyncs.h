#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API SyncLock
{
public:
	SyncLock();
	~SyncLock();

	void Enter();
	void Leave();

	class XSC_API Auto
	{
	public:
		Auto(SyncLock& syncLock);
		~Auto();

	private:
		SyncLock& mSyncLock;
	};

private:
	CRITICAL_SECTION mCricticalSection;
};

class XSC_API SyncEvent
{
public:
	SyncEvent();
	~SyncEvent();

	enum CreateReturn { CREATE_SUCCESS_NEW, CREATE_SUCCESS_ALREADY_EXISTS, CREATE_FAILURE };
	CreateReturn Create(bool autoReset, const wchar_t* name = nullptr);
	void Release();

	void Set();
	void Reset();

	bool IsSet();

	void Wait();

private:
	HANDLE mEvent;
};

} // namespace xsc
