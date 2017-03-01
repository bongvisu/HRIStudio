#pragma once

#include <xscEntity.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API LogItem
{
public:
	LogItem();
	LogItem(const LogItem& item);
	LogItem& operator=(const LogItem& item);

	LogItem(LogItem&& item);
	LogItem& operator=(LogItem&& item);

public:
	std::wstring senderName;
	xsc::Entity::ID senderID;
	std::wstring message;
};

class HSMO_API ILogConsumer
{
public:
	virtual void EnqueueLogItem(LogItem&& item) = 0;

protected:
	virtual ~ILogConsumer();
};

class HSMO_API LogMediator
{
public:
	void SetConsumer(ILogConsumer* consumer);
	void ResetConsumer();
	void TransferLogItem(LogItem&& item);

protected:
	LogMediator();
	~LogMediator();

	ILogConsumer* mConsumer;

	// ================================================================================================================
	private: friend Module;
	static void InitLogMediator();
	static void TermLogMediator();
	// ================================================================================================================
};

extern HSMO_API LogMediator* theLogMediator;

} // namespace hsmo
