#include "stdafx.h"
#include "BlackBoard.h"

UniqueIdFactory<BlackBoardUID> BlackBoard::nameFactory(1000);

BlackBoardSubscriber::~BlackBoardSubscriber()
{
	// Do nothing
}

BlackBoardSubscriber::BlackBoardSubscriber(BlackBoardUID _uid,
										   BlackBoard *_blackBoard)
: uid(_uid),
parentBlackBoard(_blackBoard)
{}

void BlackBoardSubscriber::recvMessage(Message &message)
{
	map<string, MessageHandler>::const_iterator
		i = messageHandlers.find(message.getMessageType());

	if(i != messageHandlers.end())
	{
		(i->second)(message);
	}
}

void BlackBoardSubscriber::registerMessageHandler(const string &type,
							                      const MessageHandler &fn)
{
	messageHandlers[type] = fn;
}

void BlackBoardSubscriber::setParentBlackBoard(BlackBoard *parent)
{
	ASSERT(parent, "Parameter \"parent\" was null");

	if(parentBlackBoard)
	{
		parentBlackBoard->removeSubscriber(getUID());
	}

	parentBlackBoard = parent;

	parentBlackBoard->registerSubscriber(this);
}

BlackBoard::BlackBoard()
: BlackBoardSubscriber(genName(), 0)
{
	// Do nothing
}

BlackBoard::BlackBoard(BlackBoardUID uid, BlackBoard *parentBlackBoard)
: BlackBoardSubscriber(uid, parentBlackBoard)
{
	// Do nothing
}

void BlackBoard::clear()
{
	subscribersByType.clear();
	subscribersByUid.clear();
}

BlackBoardUID BlackBoard::registerSubscriber(SubscriberPtr subscriber)
{
	ASSERT(subscriber, "Subscriber is null");

	BlackBoardUID uid = subscriber->getUID();

	if(!isSubscriber(uid))
	{
		subscribersByUid.insert(make_pair(uid, subscriber));

		const string &name = subscriber->getTypeString();
		subscribersByType.insert(make_pair(name, subscriber));
	}

	return uid;
}

SubscriberPtr BlackBoard::removeSubscriber(BlackBoardUID uid)
{
	ASSERT(isSubscriber(uid), "Can not remove subscriber!");

	SubscriberPtr r;
	
	// Remove the subscriber from the list indexed by UID
	{
		MapByUID::iterator i = subscribersByUid.find(uid);
		r = (i->second);
		subscribersByUid.erase(i);
	}

	string typeString = r->getTypeString();

	// Remove the subscriber from the list indexed by type
	MultiMapByType::iterator i=subscribersByType.find(typeString);
	while(i != subscribersByType.end())
	{
		if((i->second)->getUID() == uid)
		{
			i = subscribersByType.erase(i);
		}
		else
		{
			++i;
		}
	}

	return r;
}

void BlackBoard::recvMessage(Message &message)
{
	BlackBoardSubscriber::recvMessage(message);

	for(MapByUID::iterator i = subscribersByUid.begin();
		i != subscribersByUid.end(); ++i)
	{
		SubscriberPtr subscriber = i->second;
		ASSERT(subscriber, "Subscriber is null");
		subscriber->recvMessage(message);
	}
}

void BlackBoard::relayGlobalMessage(Message &message)
{
	if(getParentBlackBoardPtr())
	{
		getParentBlackBoardPtr()->relayGlobalMessage(message);
	}
	else
	{
		relayMessage(message);
	}
}

SubscriberPtr BlackBoard::getSubscriber(const string &name)
{
	MultiMapByType::const_iterator i = subscribersByType.find(name);

	if(i != subscribersByType.end())
	{
		return (i->second);
	}

	return SubscriberPtr();
}

const SubscriberPtr BlackBoard::getSubscriber(const string &type) const
{
	MultiMapByType::const_iterator i=subscribersByType.find(type);

	if(i != subscribersByType.end())
	{
		return (i->second);
	}

	return SubscriberPtr();
}

bool BlackBoard::hasSubscriber(const string &type) const
{
	MultiMapByType::const_iterator i = subscribersByType.find(type);

	return(i != subscribersByType.end());
}

bool BlackBoard::isSubscriber(BlackBoardUID uid) const
{
	MapByUID::const_iterator i = subscribersByUid.find(uid);

	return(i != subscribersByUid.end());
}
