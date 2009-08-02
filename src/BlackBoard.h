#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include "UniqueIdFactory.h"

typedef int BlackBoardUID;

/** Generic BlackBoard message */
class Message
{
public:
	/**
	Constructs a message that can be passed through a BlackBoard network
	@param _eventType String to uniquely identify the type of message/event
	*/
	Message(const string &_eventType)
	: eventType(_eventType)
	{
		// Do nothing
	}

	/** Gets a string that identifies the message type */
	inline const string& getMessageType() const
	{
		return eventType;
	}

	/**
	Determines whether a field of the specified name is present in the message.
	@param name Name of the field
	@return true if the field is part of the message, false otherwise
	*/
	inline bool isField(const string &name) const
	{
		return fields.find(name) != fields.end();
	}

	/**
	Adds a field to the message
	@param name Name of the field
	@param value Value of the field (copied into the message)
	*/
	inline void addField(const string &name, const any &value)
	{
		fields[name] = value;
	}

	/**
	Gets the value of a field of a certain type from the message.
	@param name Name of the field
	@return value of the field
	@throws bad_any_cast when the field could not be cast to the desired type.
	*/
	template<typename TYPE>
	inline TYPE getField(const string &name) const
	{
		ASSERT(isField(name), "Not a field: " + name);

		any value = fields.find(name)->second;

		try
		{
			return any_cast<TYPE>(value);
		}
		catch(const bad_any_cast &)
		{
			const string t = typeid(TYPE).name();
			const string u = value.type().name();
			FAIL("Field \"" + name + "\" does not have type \"" + t +
				"\", but is of type \"" + u +  "\" instead.");
			throw;
		}
	}

private:
	string eventType;
	map<string, any> fields;
};

class BlackBoard; // forward declaration for use by BlackBoardBlackBoardSubscriber

/** Base type for an object that send/receives blackboard messages */
class BlackBoardSubscriber
{
public:
	/** Function type for a message/event handler */
	typedef function< void(Message &message) > MessageHandler;

	/** Every subscriber type should have a string to identify their type */
	virtual string getTypeString() const { return "Subscriber"; }

	/** Destructor */
	virtual ~BlackBoardSubscriber();

	/**
	@param uid UID to identify this blackboard subscriber
	@param parentBlackBoard Black board that will be sending messages to this
	                        subscriber.  Note that messages may come from
							external sources *anyway*
	*/
	BlackBoardSubscriber(BlackBoardUID uid, BlackBoard *parentBlackBoard);

	/**
	Receive a message of some kind.
	@param message Some message
	*/
	virtual void recvMessage(Message &message);

	/**
	Registers a message handler for the BlackBoardSubscriber.
	MessageType is the type of the message that is handled
	@param messageHandler Message handler callback
	*/
	void registerMessageHandler(const string &type,
	                            const MessageHandler &handler);

	/** Gets the BlackBoardUID of the BlackBoardSubscriber */
	inline BlackBoardUID getUID() const
	{
		return uid;
	}

	inline BlackBoard& getParentBlackBoard() const
	{
		ASSERT(parentBlackBoard, "parentBlackBoard was null");
		return *parentBlackBoard;
	}

	inline BlackBoard* getParentBlackBoardPtr() const
	{
		return parentBlackBoard;
	}

	/** Sets the parent blackboard */
	void setParentBlackBoard(BlackBoard *parent);

private:
	BlackBoardUID uid;
	BlackBoard *parentBlackBoard;
	map<string, MessageHandler> messageHandlers;
};

typedef BlackBoardSubscriber* SubscriberPtr;

/**
System to allow anonymous communication between objects using a message
passing system.
*/
class BlackBoard : public BlackBoardSubscriber
{
public:
	/** Every subscriber type should have a string to identify their type */
	virtual string getTypeString() const { return "BlackBoard"; }

	/**
	Default Constructor
	Constructs a blackboard that is not a subscriber to a larger network
	*/
	BlackBoard();

	/**
	Constructs a blackboard that is also a subscriber to a larger network
	@param uid UID to identify this blackboard subscriber
	@param parentBlackBoard Black board that will be sending messages to this
	subscriber.  Note that messages may come from
	external sources *anyway*
	*/
	BlackBoard(BlackBoardUID uid, BlackBoard *parentBlackBoard);

	/** Clears all subscribers */
	void clear();

	/**
	Adds a new BlackBoardSubscriber to the list of those that receive messages.
	@param s New blackboard BlackBoardSubscriber
	*/
	BlackBoardUID registerSubscriber(SubscriberPtr s);

	/**
	Removes a subscriber
	@param uid UID of the subscriber to remove
	@return Pointer to the removed subscriber
	*/
	SubscriberPtr removeSubscriber(BlackBoardUID uid);

	/** Gets a the first subscriber of a given type, or returns null */
	SubscriberPtr getSubscriber(const string &type);

	/** Gets a the first subscriber of a given type, or returns null */
	const SubscriberPtr getSubscriber(const string &type) const;

	/** Determines whether the blackboard has a subscriber of a given type */
	bool hasSubscriber(const string &type) const;

	/** Determines whether the blackboard has a particular subscriber */
	bool isSubscriber(BlackBoardUID uid) const;

	/**
	Receives a message of some kind.
	Relays a message to all blackboard subscribers.
	@param message Some message
	*/
	virtual void recvMessage(Message &message);

	/**
	Receives a message of some kind.
	Relays a message to all blackboard subscribers.
	@param message Some message
	*/
	inline void relayMessage(Message &message)
	{
		recvMessage(message);
	}

	/**
	Receives a message of some kind.
	Relays a message to at the global of the blackboard network.
	@param message Some message
	*/
	void relayGlobalMessage(Message &message);

	/** Generate a UID for some subscriber */
	inline static BlackBoardUID genName()
	{
		return nameFactory.getUid();
	}

private:
	static UniqueIdFactory<BlackBoardUID> nameFactory;

	typedef map<BlackBoardUID, SubscriberPtr> MapByUID;
	typedef multimap<string, SubscriberPtr> MultiMapByType;

	MapByUID subscribersByUid;
	MultiMapByType subscribersByType;
};

#endif
