#include "stdafx.h"
#include "PickupItem_Health.h"
#include "MessageCharacterReceivesHealing.h"

void PickupItem_Health::executeEffect(OBJECT_ID id)
{
	playSound(getSound());
	relayMessage(id, MessageCharacterReceivesHealing(healing));

	// TODO: Special FX
}

void PickupItem_Health::load( const PropertyBag &data )
{
	PickupItem::load(data);
	healing = data.getInt("healing");
}
