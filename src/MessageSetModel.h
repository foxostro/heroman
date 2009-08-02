#ifndef MESSAGE_SET_MODEL_H
#define MESSAGE_SET_MODEL_H

#include "BlackBoard.h"

/** Message to request that the object's model be changed */
class MessageRequestSetModel : public Message
{
public:
	MessageRequestSetModel(const FileName& fileName)
	: Message("MessageRequestSetModel")
	{
		addField("fileName", fileName);
	}
};

#endif
