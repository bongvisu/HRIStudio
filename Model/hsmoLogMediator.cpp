#include "hsmoPrecompiled.h"
#include "hsmoLogMediator.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ================================================================================================================
LogMediator* theLogMediator;

void LogMediator::InitLogMediator()
{
	theLogMediator = new LogMediator;
}

void LogMediator::TermLogMediator()
{
	delete theLogMediator;
}
// ================================================================================================================

LogMediator::LogMediator()
	:
	mConsumer(nullptr)
{
}

LogMediator::~LogMediator()
{
}

void LogMediator::SetConsumer(ILogConsumer* consumer)
{
	mConsumer = consumer;
}

void LogMediator::ResetConsumer()
{
	mConsumer = nullptr;
}

void LogMediator::TransferLogItem(LogItem&& item)
{
	if (mConsumer)
	{
		mConsumer->EnqueueLogItem(std::move(item));
	}
}

LogItem::LogItem()
{
	senderID = Entity::NULL_ID;
}

LogItem::LogItem(const LogItem& item)
{
	*this = item;
}

LogItem& LogItem::operator=(const LogItem& item)
{
	senderName = item.senderName;
	senderID = item.senderID;
	message = item.message;
	return *this;
}

LogItem::LogItem(LogItem&& item)
{
	*this = std::move(item);
}

LogItem& LogItem::operator=(LogItem&& item)
{
	senderName = std::move(item.senderName);
	senderID = item.senderID;
	message = std::move(item.message);
	return *this;
}

ILogConsumer::~ILogConsumer()
{
}

} // namespace hsmo
