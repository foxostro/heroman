#include "stdafx.h"
#include "Actor.h"
#include "ActorFactory.h"

ActorFactory& getActorFactory()
{
	static ActorFactory *factory = 0;
	
	if(!factory)
	{
		// Create on first call
		factory = new ActorFactory();
	}

	ASSERT(factory, "Factory pointer was null");
	return(*factory);
}
