#include "stdafx.h"
#include "Actor.h"
#include "MessageClientUsesObject.h"
#include "ComponentSwitchReceiver.h"

ComponentSwitchReceiver::ComponentSwitchReceiver(BlackBoardUID uid,
                                                 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	registerMessageHandler("MessageSwitchWasToggled", bind(&ComponentSwitchReceiver::handleMessageSwitchWasToggled, this, _1));
}

void ComponentSwitchReceiver::handleMessageSwitchWasToggled(Message &message)
{
	OBJECT_ID requesterID = message.getField<OBJECT_ID>("requesterID");
	int categoryID = message.getField<int>("categoryID");
	
	ASSERT(categoryID<MAX_SWITCHES && categoryID>=0, "\"categoryID\" was invalid!");	
	switchState[categoryID] = !switchState[categoryID];

	if(expr_on_switch.eval<bool>())
	{
		for(int i=0; i<MAX_SWITCHES; ++i)
		{
			switchStateOld[i] = switchState[i];
		}

		OBJECT_ID requesterID = message.getField<OBJECT_ID>("requesterID");
		getParentBlackBoard().relayMessage(MessageClientUsesObject(requesterID));
	}
}

void ComponentSwitchReceiver::resetMembers()
{
	expr_on_switch = sexpr_t();

	for(int i=0; i<MAX_SWITCHES; ++i)
	{
		switchState[i] = switchStateOld[i] = false;
	}
}

sexpr_t::tree_node_ptr ComponentSwitchReceiver::defun_switchState(sexpr_t::cells_t parameters)
{
	const int switchIdx = sexpr_t::eval_leaf<int>(*parameters.begin());
	ASSERT(switchIdx<MAX_SWITCHES && switchIdx>=0, "\"switchIdx\" invalid" + itos(switchIdx));
	const bool value = switchState[switchIdx];
	return sexpr_t::tree_node_ptr(new sexpr_t::tree_leaf_t<bool>(value));
}

sexpr_t::tree_node_ptr ComponentSwitchReceiver::defun_switchToggled(sexpr_t::cells_t parameters)
{
	const int switchIdx = sexpr_t::eval_leaf<int>(*parameters.begin());
	ASSERT(switchIdx<MAX_SWITCHES && switchIdx>=0, "\"switchIdx\" invalid" + itos(switchIdx));
	const bool curr = switchState[switchIdx];
	const bool prev = switchStateOld[switchIdx];
	return sexpr_t::tree_node_ptr(new sexpr_t::tree_leaf_t<bool>(curr != prev));
}

void ComponentSwitchReceiver::load(const PropertyBag &data)
{
	resetMembers();
	loadExpressions(data);
}

void ComponentSwitchReceiver::loadExpressions(const PropertyBag &data)
{
	expr_on_switch = sexpr_t::fromString(data.getString("expr_on_switch"));
	expr_on_switch.addBinding("switchState",   bind(&ComponentSwitchReceiver::defun_switchState, this, _1));
	expr_on_switch.addBinding("switchToggled", bind(&ComponentSwitchReceiver::defun_switchToggled, this, _1));
}

void ComponentSwitchReceiver::update(float milliseconds)
{}
