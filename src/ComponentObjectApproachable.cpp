#include "stdafx.h"
#include "World.h"
#include "MessageClientApproachesObject.h"
#include "MessageClientRecedesFromObject.h"
#include "ComponentPhysics.h"
#include "ComponentObjectApproachable.h"

ComponentObjectApproachable::ComponentObjectApproachable( BlackBoardUID uid, BlackBoard *blackBoard )
: Component(uid, blackBoard),
  world(0),
  thresholdTrigger(0.0f),
  thresholdRelease(0.0f)
{
	registerMessageHandler("MessagePositionHasBeenSet", bind(&ComponentObjectApproachable::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageSetWorld",           bind(&ComponentObjectApproachable::handleMessageSetWorld, this, _1));
	resetMembers();
}

void ComponentObjectApproachable::resetMembers()
{
	thresholdTrigger = 1.0f;
	thresholdRelease = 2.0f;

	playerState.clear();
}

void ComponentObjectApproachable::load( const PropertyBag &data )
{
	resetMembers();
	thresholdTrigger = data.getFloat("thresholdTrigger");
	thresholdRelease = data.getFloat("thresholdRelease");
}

void ComponentObjectApproachable::update( float milliseconds )
{
	ASSERT(world, "World has not been set yet");

	vector<ActorPtr> &players = world->players;

	for(vector<ActorPtr>::iterator i=players.begin(); i!=players.end(); ++i)
	{
		ActorPtr player = *i;

		if(player->hasComponent("Physics"))
		{
			ComponentPhysics *physics = dynamic_cast<ComponentPhysics*>(player->getComponent("Physics"));

			if(physics)
			{
				const float distance = vec3((physics->getPosition())-lastReportedPosition).getMagnitude();
				const OBJECT_ID id = player->getUID();

				if(distance <= thresholdTrigger)
				{
					playerApproaches(id);
				}
				else if(distance > thresholdRelease)
				{
					playerRecedes(id);
				}
			}
		}
	}
}

void ComponentObjectApproachable::handleMessagePositionHasBeenSet( Message &message )
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentObjectApproachable::handleMessageSetWorld( Message &message )
{
	world = message.getField<World*const>("world");
}

void ComponentObjectApproachable::playerRecedes(OBJECT_ID id)
{
	if(playerState[id] == OBJECT_NEAR)
	{
		playerState[id] = OBJECT_FAR;
		getParentBlackBoard().relayMessage(MessageClientRecedesFromObject(id));
	}
}

void ComponentObjectApproachable::playerApproaches(OBJECT_ID id)
{
	if(playerState[id] == OBJECT_FAR)
	{
		playerState[id] = OBJECT_NEAR;
		getParentBlackBoard().relayMessage(MessageClientApproachesObject(id));
	}
}
