#ifndef PICKUPITEM_POWER_H
#define PICKUPITEM_POWER_H

#include "Pickups.h"

class PickupItem_Power : public PickupItem
{
public:
	/**
	Constructor
	@param world Game world
	*/
	PickupItem_Power(World *world)
	: PickupItem(world),
	  power(0)
	{}

	/** Performs the effect associated with the pickup of this item */
	virtual void executeEffect(OBJECT_ID id);

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

private:
	int power;
};

#endif
