#include "stdafx.h"
#include "MessageCharacterReceivesWeaponPower.h"
#include "PickupItem_Power.h"

void PickupItem_Power::executeEffect(OBJECT_ID id)
{
	playSound(getSound());
	relayMessage(id, MessageCharacterReceivesWeaponPower(power));

	// TODO: Special FX
}

void PickupItem_Power::load( const PropertyBag &data )
{
	PickupItem::load(data);
	power = data.getInt("power");
}
